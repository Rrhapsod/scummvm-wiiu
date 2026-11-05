// Analytic input trajectories exercise the actual integrated SDL helper.
// They measure synthetic tracking, not Wii U Menu behavior or physical IR.
static float normalized(float pixel, int extent) {
    return 2.0f * pixel / (extent - 1) - 1.0f;
}
static void feed(float x, float y, Uint32 ms = 16, Uint32 held = 0, int valid = 1) {
    update(0, normalized(x, window.w), normalized(y, window.h), held, valid, ms);
}
static void resetTracking(int width, int height, float x, float y, Uint32 held = 0) {
    WIIU_DisableRemotePointer(0);
    window.w = width; window.h = height; window.flags = SDL_WINDOW_SHOWN;
    ticks += 1000;
    WIIU_ConfigureRemotePointer(0, WPAD_EXT_CORE);
    feed(x,y,16,held);
    mouseEvents.clear();
}
static void stabilityCases() {
    // Fixed-rate 5/11 Hz noise with a zero-mean envelope, on both axes.
    // Deliberately crosses several integer pixels, unlike the previous tests.
    for (const auto &dimensions : std::vector<std::pair<int,int>>{{1280,720},{1920,1080},{854,480}}) {
        const int w=dimensions.first,h=dimensions.second;
        for (Uint32 ms : {8u,16u,33u}) {
            const float scale=(w-1)/1279.0f, cx=w*.5f+.375f,cy=h*.5f+.375f;
            resetTracking(w,h,cx,cy);
            double rawEnergy=0,filteredEnergy=0,sumX=0,sumY=0;
            int lowX=w,highX=0;
            for (int i=1;i<=600;++i) {
                const float t=i*ms*.001f;
                const float nx=scale*(3.0f*std::sin(31.4159265f*t)+1.0f*std::sin(69.1150384f*t));
                const float ny=scale*(3.0f*std::cos(31.4159265f*t)+1.0f*std::cos(69.1150384f*t));
                feed(cx+nx,cy+ny,ms);
                if (i>100) {
                    rawEnergy+=nx*nx+ny*ny;
                    // Compare integer outputs with the same quantization oracle.
                    const double ex=mx-(int)cx,ey=my-(int)cy;
                    filteredEnergy+=ex*ex+ey*ey;
                    sumX+=ex;sumY+=ey;
                    lowX=std::min(lowX,mx);highX=std::max(highX,mx);
                }
            }
            // Stationary jitter is variation around the mean; a fixed
            // integer-quantization offset is not cursor tremor.
            filteredEnergy-=(sumX*sumX+sumY*sumY)/500.0;
            const double ratio=std::sqrt(filteredEnergy/rawEnergy);
            printf("IR stationary noise %dx%d @%ums: RMS ratio %.3f; x span %d pixels\n",w,h,ms,ratio,highX-lowX);
            fflush(stdout);
            assert(ratio<.35); // >=65% RMS attenuation, including output quantization
            assert(highX-lowX<=std::ceil(2.0f*scale));
        }
    }
    // Quarter-pixel input steps at a realistic 16 ms cadence: 15.625 px/s.
    // Allow bounded smoothing lag, require continuous one-pixel output steps.
    for (const auto &dimensions : std::vector<std::pair<int,int>>{{1280,720},{1920,1080},{854,480},{320,200}}) {
        const int w=dimensions.first,h=dimensions.second;
        for (int axis=0;axis<3;++axis) for(int direction : {-1,1}) {
            const Uint32 held=direction<0?WPAD_BUTTON_A:0;
            resetTracking(w,h,100.375f,100.375f,held);
            int lastX=mx,lastY=my,maxStep=0,motions=0;
            double maxLag=0;
            for(int i=1;i<=256;++i) {
                const float target=100.375f+direction*i*.25f;
                feed(axis!=1?target:100.375f,axis!=0?target:100.375f,16,held);
                maxStep=std::max(maxStep,std::max(std::abs(mx-lastX),std::abs(my-lastY)));
                if(mx!=lastX||my!=lastY)++motions;
                const double lag=std::abs((axis!=1?mx:my)-target);
                if(i>60)maxLag=std::max(maxLag,lag);
                lastX=mx;lastY=my;
            }
            printf("IR slow ramp %dx%d axis=%d dir=%d: %d moves; max step %d; lag %.2f pixels\n",w,h,axis,direction,motions,maxStep,maxLag);
            fflush(stdout);
            assert(maxStep==1 && motions>=60 && maxLag<4.0);
            for(const auto &event:mouseEvents)assert(event.type==0);
            const float end=100.375f+direction*64.0f;
            for(int i=0;i<120;++i)feed(axis!=1?end:100.375f,axis!=0?end:100.375f,16,held);
            assert(mx==(int)(axis!=1?end:100.375f) && my==(int)(axis!=0?end:100.375f));
        }
    }
    // Quick motion and reversal: bound steady tracking delay to 32 ms.
    for(Uint32 ms : {8u,16u,33u}) {
        resetTracking(1280,720,200.375f,300.375f);
        double worst=0;
        for(int i=1;i<=90;++i) {
            const float x=200.375f+300.0f*i*ms*.001f;
            feed(x,300.375f,ms);
            if(i>20)worst=std::max(worst,(x-mx)/300.0);
        }
        printf("IR quick ramp @%ums: steady delay <= %.1f ms\n",ms,worst*1000.0);fflush(stdout);
        assert(worst<.032);
        const int start=mx;
        feed(start-80.0f,300.375f,ms);
        assert(mx<start); // reversal is not held by a stationary dead zone
    }
    // No integration per poll: repeated samples in the same millisecond do
    // not accelerate convergence. Button presses still arrive immediately.
    resetTracking(1280,720,400.375f,300.375f);
    feed(405.375f,300.375f);
    const int fixed=mx;
    for(int i=0;i<50;++i)feed(405.375f,300.375f,0);
    assert(mx==fixed);
    feed(405.375f,300.375f,0,WPAD_BUTTON_A);
    assert(mx==fixed && mouseEvents.back().type==1 && mouseEvents.back().state==SDL_PRESSED);
    feed(405.375f,300.375f,0);
    assert(mouseEvents.back().state==SDL_RELEASED);
    // Filter state never bridges invalid tracking, a resize, long frame gap,
    // or an input handover. Reacquisition immediately anchors the new sample.
    feed(0,0,16,0,0);feed(800.375f,500.375f);
    assert(mx==800 && my==500);
    feed(200.375f,100.375f,300);assert(mx==200 && my==100);
    window.w=1920;window.h=1080;feed(1200.375f,700.375f);
    assert(mx==1200 && my==700);
    WIIU_YieldRemotePointer();feed(1200.375f,700.375f);
    mx=30;my=40;ticks+=400;feed(1200.375f,700.375f);
    assert(remotePointerOwner==-1 && mx==30 && my==40);
    feed(1500.375f,900.375f);assert(mx==1500 && my==900);
    WIIU_SuspendRemotePointers();feed(600.375f,400.375f);
    assert(mx==600 && my==400);
    ticks=0xfffffff0u;feed(700.375f,400.375f,300);
    feed(701.375f,400.375f,16);assert(mx>=700 && mx<=701);
}
