/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#if defined(POSIX) || defined(PSP2) || defined(__DS__) || defined(__WIIU__)

#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "backends/fs/posix-drives/posix-drives-fs.h"
#include "backends/fs/posix/posix-iostream.h"

#include "common/algorithm.h"
#include "common/bufferedstream.h"

#include <dirent.h>

#ifdef __WIIU__
// WUT's cwd includes its libc device prefix (fs:/vol/external01), while
// registered drives and configuration defaults use /vol/external01. Keep
// those aliases identical for parent navigation; usb: is a different driver.
static Common::String normalizeWiiUDrivePath(const Common::String &path) {
	if (path == "fs:")
		return "/";
	if (path.hasPrefix("fs:/"))
		return Common::String(path.c_str() + 3);
	return path;
}
#endif

DrivePOSIXFilesystemNode::DrivePOSIXFilesystemNode(const Common::String &path, const Config &config) :
#ifdef __WIIU__
		POSIXFilesystemNode(normalizeWiiUDrivePath(path)),
#else
		POSIXFilesystemNode(path),
#endif
		_isPseudoRoot(false),
		_config(config) {

#ifdef __WIIU__
	// Give the drive list a persistent, editable path. A physical FSA root
	// would show only 'vol' and have no parent, trapping legacy browser paths.
	if (_path == "/") {
		_isPseudoRoot = true;
		_isDirectory = true;
		_isValid = true;
		return;
	}
#endif

#ifdef __WIIU__
	if (isDrive(_path)) {
#else
	if (isDrive(path)) {
#endif
		_isDirectory = true;
		_isValid = true;

		// FIXME: Add a setting for deciding whether drive paths need to end with a slash
		if (!_path.hasSuffix("/")) {
			_path += "/";
		}
		return;
	}
}

DrivePOSIXFilesystemNode::DrivePOSIXFilesystemNode(const Config &config) :
		_isPseudoRoot(true),
		_config(config) {
	_isDirectory = true;
	_isValid = false;
#ifdef __WIIU__
	_path = "/";
	_displayName = "/";
#endif
}

void DrivePOSIXFilesystemNode::configureStream(StdioStream *stream) {
	if (!stream) {
		return;
	}

	if (_config.bufferingMode != kBufferingModeStdio) {
		// Disable stdio buffering
		stream->setBufferSize(0);
	} else if (_config.bufferSize) {
		stream->setBufferSize(_config.bufferSize);
	}
}

Common::SeekableReadStream *DrivePOSIXFilesystemNode::createReadStream() {
	StdioStream *readStream = PosixIoStream::makeFromPath(getPath(), StdioStream::WriteMode_Read);

	configureStream(readStream);

	if (_config.bufferingMode == kBufferingModeScummVM) {
		uint32 bufferSize = _config.bufferSize != 0 ? _config.bufferSize : 1024;
		return Common::wrapBufferedSeekableReadStream(readStream, bufferSize, DisposeAfterUse::YES);
	}

	return readStream;
}

Common::SeekableWriteStream *DrivePOSIXFilesystemNode::createWriteStream(bool atomic) {
	StdioStream *writeStream = PosixIoStream::makeFromPath(getPath(), atomic ?
			StdioStream::WriteMode_WriteAtomic : StdioStream::WriteMode_Write);

	configureStream(writeStream);

	if (_config.bufferingMode == kBufferingModeScummVM) {
		uint32 bufferSize = _config.bufferSize != 0 ? _config.bufferSize : 1024;
		return Common::wrapBufferedWriteStream(writeStream, bufferSize);
	}

	return writeStream;
}

DrivePOSIXFilesystemNode *DrivePOSIXFilesystemNode::getChildWithKnownType(const Common::String &n, bool isDirectoryFlag) const {
	assert(_isDirectory);

	// Make sure the string contains no slashes
	assert(!n.contains('/'));

	Common::String newPath(_path);
	if (_path.lastChar() != '/')
		newPath += '/';
	newPath += n;

	DrivePOSIXFilesystemNode *child = reinterpret_cast<DrivePOSIXFilesystemNode *>(makeNode());
	child->_path = newPath;
	child->_isValid = true;
	child->_isPseudoRoot = false;
	child->_isDirectory = isDirectoryFlag;
	child->_displayName = n;

	return child;
}

AbstractFSNode *DrivePOSIXFilesystemNode::getChild(const Common::String &n) const {
	DrivePOSIXFilesystemNode *child = getChildWithKnownType(n, false);
	child->setFlags();

	return child;
}

bool DrivePOSIXFilesystemNode::getChildren(AbstractFSList &list, AbstractFSNode::ListMode mode, bool hidden) const {
	assert(_isDirectory);

	if (_isPseudoRoot) {
		return _config.getDrives(list, hidden);
	} else {
		DIR *dirp = opendir(_path.c_str());
		struct dirent *dp;

		if (!dirp)
			return false;

		while ((dp = readdir(dirp)) != nullptr) {
			// Skip 'invisible' files if necessary
			if (dp->d_name[0] == '.' && !hidden) {
				continue;
			}

			// Skip '.' and '..' to avoid cycles
			if ((dp->d_name[0] == '.' && dp->d_name[1] == 0) || (dp->d_name[0] == '.' && dp->d_name[1] == '.')) {
				continue;
			}

			AbstractFSNode *child = nullptr;

#if !defined(SYSTEM_NOT_SUPPORTING_D_TYPE)
			if (dp->d_type == DT_DIR || dp->d_type == DT_REG) {
				child = getChildWithKnownType(dp->d_name, dp->d_type == DT_DIR);
			} else
#endif
			{
				child = getChild(dp->d_name);
			}

			// Honor the chosen mode
			if ((mode == Common::FSNode::kListFilesOnly && child->isDirectory()) ||
				(mode == Common::FSNode::kListDirectoriesOnly && !child->isDirectory())) {
				delete child;
				continue;
			}

			list.push_back(child);
		}

		closedir(dirp);

		return true;
	}
}

AbstractFSNode *DrivePOSIXFilesystemNode::getParent() const {
	if (_isPseudoRoot) {
		return nullptr;
	}

	if (isDrive(_path)) {
		return makeNode();
	}

	return POSIXFilesystemNode::getParent();
}

bool DrivePOSIXFilesystemNode::isDrive(const Common::String &path) const {
	Common::String normalizedPath = Common::normalizePath(path, '/');
	return _config.isDrive(normalizedPath);
}

#endif //#if defined(POSIX)
