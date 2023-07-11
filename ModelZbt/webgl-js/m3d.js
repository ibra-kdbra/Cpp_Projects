m3d={
    /* shaders */
    vs:"attribute vec3 v;\nattribute vec3 n;\nattribute vec4 c;\nuniform mat4 m;\nuniform mat4 p;\nvarying vec3 V;\nvarying vec3 N;\nvarying vec4 C;\nvoid main(void) {\nvec4 w = vec4(v, 1.0);\ngl_Position = p * m * w;\nV = vec3(m * w);\nN = vec3(m * vec4(n, 0));\nC = c;\n}",
    fs:"precision highp float;\nvarying vec3 V;\nvarying vec3 N;\nvarying vec4 C;\nvoid main(void) {\nvec3 no = normalize(N);\nvec3 l = normalize(vec3(-1,2,2)-V);\nfloat d = max(dot(no,l), 0.0);\ngl_FragColor = vec4(d * 1.1 * vec3(1.0,1.0,0.5) * vec3(C), C.w);\n}\n",

    /* matrix functions */
    identity:function(){
        var i,r=new Float32Array(16);
        for(i=0;i<16;i++)r[0]=0.0;
        r[0]=r[5]=r[10]=r[15]=1.0;
        return r;
    },
    rotate:function(a,b){
        var r=new Float32Array(16),x=b[0],y=b[1],z=b[2],d=Math.sqrt(x*x+y*y+z*z),s,c,t;
        if(!d)return null;
        x/=d;y/=d;z/=d;
        s=Math.sin(a);
        c=Math.cos(a);
        t=1-c;
        r[0]=x*x*t+c;r[1]=y*x*t+z*s;r[2]=z*x*t-y*s;
        r[4]=x*y*t-z*s;r[5]=y*y*t+c;r[6]=z*y*t+x*s;
        r[8]=x*z*t+y*s;r[9]=y*z*t-x*s;
        r[10]=z*z*t+c;
        r[3]=r[7]=r[11]=r[12]=r[13]=r[14]=0.0;
        r[15]=1.0;
        return r;
    },
    multiply:function(a,b){
        var r=new Float32Array(16);
        r[0]=b[0]*a[0]+b[1]*a[4]+b[2]*a[8]+b[3]*a[12];
        r[1]=b[0]*a[1]+b[1]*a[5]+b[2]*a[9]+b[3]*a[13];
        r[2]=b[0]*a[2]+b[1]*a[6]+b[2]*a[10]+b[3]*a[14];
        r[3]=b[0]*a[3]+b[1]*a[7]+b[2]*a[11]+b[3]*a[15];
        r[4]=b[4]*a[0]+b[5]*a[4]+b[6]*a[8]+b[7]*a[12];
        r[5]=b[4]*a[1]+b[5]*a[5]+b[6]*a[9]+b[7]*a[13];
        r[6]=b[4]*a[2]+b[5]*a[6]+b[6]*a[10]+b[7]*a[14];
        r[7]=b[4]*a[3]+b[5]*a[7]+b[6]*a[11]+b[7]*a[15];
        r[8]=b[8]*a[0]+b[9]*a[4]+b[10]*a[8]+b[11]*a[12];
        r[9]=b[8]*a[1]+b[9]*a[5]+b[10]*a[9]+b[11]*a[13];
        r[10]=b[8]*a[2]+b[9]*a[6]+b[10]*a[10]+b[11]*a[14];
        r[11]=b[8]*a[3]+b[9]*a[7]+b[10]*a[11]+b[11]*a[15];
        r[12]=b[12]*a[0]+b[13]*a[4]+b[14]*a[8]+b[15]*a[12];
        r[13]=b[12]*a[1]+b[13]*a[5]+b[14]*a[9]+b[15]*a[13];
        r[14]=b[12]*a[2]+b[13]*a[6]+b[14]*a[10]+b[15]*a[14];
        r[15]=b[12]*a[3]+b[13]*a[7]+b[14]*a[11]+b[15]*a[15];
        return r;
    },
    perspective:function(y,a,n,f){
        var t=n*Math.tan(y*Math.PI/360.0),b=-t,ri=t*a,l=-ri;
        var rl=(ri-l),tb=(t-b),fn=(f-n),r=new Float32Array(16);
        r[0]=(n*2)/rl;r[5]=(n*2)/tb;
        r[8]=(ri+l)/rl;r[9]=(t+b)/tb;r[10]=-(f+n)/fn;r[11]=-1;
        r[14]=-(f*n*2)/fn;
        r[1]=r[2]=r[3]=r[4]=r[6]=r[7]=r[12]=r[13]=r[15]=0;
        return r;
    },
    /* renderer */
    render:function(canvas){
        var gl=canvas.gl;
        gl.viewport(0,0,gl.viewportWidth,gl.viewportHeight);
        gl.clear(gl.COLOR_BUFFER_BIT|gl.DEPTH_BUFFER_BIT);
        gl.useProgram(canvas.s);
        gl.uniformMatrix4fv(canvas.s.pu,false,canvas.p);
        gl.uniformMatrix4fv(canvas.s.mu,false,canvas.m);
        gl.bindBuffer(gl.ARRAY_BUFFER,canvas.v);
        gl.vertexAttribPointer(canvas.s.pa,3,gl.FLOAT,false,10*4,0);
        gl.bindBuffer(gl.ARRAY_BUFFER,canvas.c);
        gl.vertexAttribPointer(canvas.s.ca,4,gl.FLOAT,false,10*4,3*4);
        gl.bindBuffer(gl.ARRAY_BUFFER,canvas.n);
        gl.vertexAttribPointer(canvas.s.na,3,gl.FLOAT,false,10*4,7*4);
        gl.drawArrays(gl.TRIANGLES,0,canvas.i);
    },
    /* handle rotation events */
    drag:function(e){
        e.target.mx=e.clientX;
        e.target.my=e.clientY;
        e.target.d=true;
        e.preventDefault();
    },
    release:function(e){
        e.target.d=false;
        e.preventDefault();
    },
    move:function(e){
        e.preventDefault();
        if(e.type=="mousemove"&&!e.buttons)e.target.d=false;
        if(!e.target.d)return;
        var dx=e.clientX-e.target.mx,dy=e.target.my-e.clientY;
        e.target.mx=e.clientX;
        e.target.my=e.clientY;
        var r=m3d.rotate(Math.sqrt(dx*dx+dy*dy)*Math.PI/180,[-dy,dx,0]);
        if(r){
            var t=m3d.identity();
            t[14]=-5.0;
            e.target.r=m3d.multiply(r,e.target.r);
            e.target.m=m3d.multiply(t,e.target.r);
            m3d.render(e.target);
        }
    },
    /* decode M3D */
    /* this is pure evil, fucked up JS mess. We are using the SAME bytes in memory for Christ's sake! */
    ba2int8:function(ba){var u=new Int8Array(ba.slice(0,1));return u[0];},
    ba2int16:function(ba){var u=new Int16Array(ba.slice(0,2));return u[0];},
    ba2int32:function(ba){var u=new Int32Array(ba.slice(0,4));return u[0];},
    ba2uint8:function(ba){var u=new Uint8Array(ba.slice(0,1));return u[0];},
    ba2uint16:function(ba){var u=new Uint16Array(ba.slice(0,2));return u[0];},
    ba2uint32:function(ba){var u=new Uint32Array(ba.slice(0,4));return u[0];},
    ba2float:function(ba){var u=new Float32Array(ba.slice(0,4));return u[0];},
    ba2str:function(ba,o,s){return String.fromCharCode.apply(null,new Uint8Array(ba.slice(o,s)));},

    load:function(canvas,g,b){
        var vc_s,vi_s,si_s,ci_s,ti_s,sk_s,vd_s,vp_s,i,j,k,l,n,m,nv=0,nt=0,nm=false,ma,c,cm=[],v=[],f=[],vt=[],vd=[];
        var px,py,pz,sx,sy,sz,x,y,z,mix,miy,miz,max,may,maz;
        if(m3d.ba2str(b,0,4)!="3DMO")return;
        b=b.slice(8);
        if(m3d.ba2str(b,0,4)=="PRVW"){
            l=m3d.ba2uint32(b.slice(4,8));
            b=b.slice(l);
        }
        if(m3d.ba2str(b,0,4)!="HEAD"){
            try{b=pako.inflate(b).buffer;}catch(e){console.log("Unable to uncompress",e);return;}
            if(m3d.ba2str(b,0,4)!="HEAD")return;
        }
        l=m3d.ba2uint32(b.slice(4,8));
        n=m3d.ba2uint8(b.slice(12,13));
        vc_s=1<<((n>>0)&3);
        vi_s=1<<((n>>2)&3);
        si_s=1<<((n>>4)&3);
        ci_s=1<<((n>>6)&3);
        n=m3d.ba2uint8(b.slice(13,14));
        ti_s=1<<((n>>0)&3);
        sk_s=1<<((n>>6)&3);
        n=m3d.ba2uint8(b.slice(14,15));
        vd_s=1<<((n>>6)&3);
        n=m3d.ba2uint8(b.slice(15,16));
        vp_s=1<<((n>>0)&3);
        if(ci_s==8)ci_s=0;
        if(sk_s==8)sk_s=0;
        mix=miy=miz=2147483647;max=may=maz=-2147483647;
        while(l&&b.byteLength&&m3d.ba2str(b,0,4)!="OMD3"){
            b=b.slice(l);
            ma=m3d.ba2str(b,0,4);
            l=m3d.ba2uint32(b.slice(4,8));
            if(l<8)break;
            c=b.slice(8,l);
            if(ma=="CMAP"){cm=new Uint8Array(c);}else
            if(ma=="VRTS"){
                while(c.byteLength){
                    switch(vc_s){
                        case 1:
                            v.push(m3d.ba2int8(c)/127.0);c=c.slice(1);
                            v.push(m3d.ba2int8(c)/127.0);c=c.slice(1);
                            v.push(m3d.ba2int8(c)/127.0);c=c.slice(2);
                        break;
                        case 2:
                            v.push(m3d.ba2int16(c)/32767.0);c=c.slice(2);
                            v.push(m3d.ba2int16(c)/32767.0);c=c.slice(2);
                            v.push(m3d.ba2int16(c)/32767.0);c=c.slice(4);
                        break;
                        case 4:
                            v.push(m3d.ba2float(c));c=c.slice(4);
                            v.push(m3d.ba2float(c));c=c.slice(4);
                            v.push(m3d.ba2float(c));c=c.slice(8);
                        break;
                    }
                    col=[0x80,0x55,0x23,0xFF];
                    switch(ci_s){
                        case 1:n=m3d.ba2uint8(c);c=c.slice(1);col=[cm[n*4],cm[n*4+1],cm[n*4+2],cm[n*4+3]];break;
                        case 2:n=m3d.ba2uint16(c);c=c.slice(2);col=[cm[n*4],cm[n*4+1],cm[n*4+2],cm[n*4+3]];break;
                        case 4:col=[
                            m3d.ba2uint8(c.slice(0,1)),
                            m3d.ba2uint8(c.slice(1,2)),
                            m3d.ba2uint8(c.slice(2,3)),
                            m3d.ba2uint8(c.slice(3,4))];
                            c=c.slice(4);
                        break;
                    }
                    v.push(col[0]/255.0);
                    v.push(col[1]/255.0);
                    v.push(col[2]/255.0);
                    v.push(col[3]/255.0);
                    if(sk_s!=8)c=c.slice(sk_s);
                    nv++;
                }
            }else
            if(ma=="MESH"){
                while(c.byteLength){
                    var a=[];
                    m=m3d.ba2uint8(c);
                    c=c.slice(1);
                    if(!(m>>4)){c=c.slice(si_s);continue;}
                    for(i=0;i<(m>>4);i++){
                        n=-1;
                        switch(vi_s){
                            case 1:n=m3d.ba2uint8(c);c=c.slice(1);break;
                            case 2:n=m3d.ba2uint16(c);c=c.slice(2);break;
                            case 4:n=m3d.ba2uint32(c);c=c.slice(4);break;
                        }
                        a.push(n);
                        if((m&1)&&ti_s!=8)c=c.slice(ti_s);
                        n=-1;
                        if(m&2){
                            switch(vi_s){
                                case 1:n=m3d.ba2uint8(c);c=c.slice(1);break;
                                case 2:n=m3d.ba2uint16(c);c=c.slice(2);break;
                                case 4:n=m3d.ba2uint32(c);c=c.slice(4);break;
                            }
                        }
                        if(n==-1)nm=true;
                        a.push(n);
                    }
                    f.push(a);
                }
            }else
            if(ma=="VOXT"){
                while(c.byteLength){
                    col=[0x80,0x55,0x23,0xFF];
                    switch(ci_s){
                        case 1:n=m3d.ba2uint8(c);c=c.slice(1);col=[cm[n*4],cm[n*4+1],cm[n*4+2],cm[n*4+3]];break;
                        case 2:n=m3d.ba2uint16(c);c=c.slice(2);col=[cm[n*4],cm[n*4+1],cm[n*4+2],cm[n*4+3]];break;
                        case 4:col=[
                            m3d.ba2uint8(c.slice(0,1)),
                            m3d.ba2uint8(c.slice(1,2)),
                            m3d.ba2uint8(c.slice(2,3)),
                            m3d.ba2uint8(c.slice(3,4))];
                            c=c.slice(4);
                        break;
                    }
                    vt.push(col[0]/255.0);
                    vt.push(col[1]/255.0);
                    vt.push(col[2]/255.0);
                    vt.push(col[3]/255.0);
                    if(si_s!=8)c=c.slice(si_s);
                    c=c.slice(2);n=m3d.ba2uint8(c);c=c.slice(1);
                    if(sk_s!=8)c=c.slice(sk_s);
                    if(n>0&&si_s!=8)c=c.slice(n*(2+si_s));
                    nt++;
                }
            }else
            if(ma=="VOXD"){
                if(si_s!=8)c=c.slice(si_s);
                px=py=pz=sx=sy=sz=x=y=z=0;
                switch(vd_s){
                    case 1:
                        px=m3d.ba2int8(c);c=c.slice(1);
                        py=m3d.ba2int8(c);c=c.slice(1);
                        pz=m3d.ba2int8(c);c=c.slice(1);
                        sx=m3d.ba2uint8(c);c=c.slice(1);
                        sy=m3d.ba2uint8(c);c=c.slice(1);
                        sz=m3d.ba2uint8(c);c=c.slice(1);
                    break;
                    case 2:
                        px=m3d.ba2int16(c);c=c.slice(2);
                        py=m3d.ba2int16(c);c=c.slice(2);
                        pz=m3d.ba2int16(c);c=c.slice(2);
                        sx=m3d.ba2uint16(c);c=c.slice(2);
                        sy=m3d.ba2uint16(c);c=c.slice(2);
                        sz=m3d.ba2uint16(c);c=c.slice(2);
                    break;
                    case 4:
                        px=m3d.ba2int32(c);c=c.slice(4);
                        py=m3d.ba2int32(c);c=c.slice(4);
                        pz=m3d.ba2int32(c);c=c.slice(4);
                        sx=m3d.ba2uint32(c);c=c.slice(4);
                        sy=m3d.ba2uint32(c);c=c.slice(4);
                        sz=m3d.ba2uint32(c);c=c.slice(4);
                    break;
                }
                if(px<mix)mix=px;if(py<miy)miy=py;if(pz<miz)miz=pz;
                if(px+sx>max)max=px+sx;if(py+sy>may)may=py+sy;if(pz+sz>maz)maz=pz+sz;
                c=c.slice(2);
console.log(vp_s);
                while(c.byteLength){
                    m=m3d.ba2uint8(c);c=c.slice(1);k=(m&127)+1;
                    if(m>127){if(vp_s==1){n=m3d.ba2uint8(c);c=c.slice(1);}else{n=m3d.ba2uint16(c);c=c.slice(2);}}
                    for(j=0;j<k;j++){
                        if(m<128){if(vp_s==1){n=m3d.ba2uint8(c);c=c.slice(1);}else{n=m3d.ba2uint16(c);c=c.slice(2);}}
                        if(n<nt){
                            vd.push(px+x);vd.push(py+y);vd.push(pz+z);vd.push(n);
                        }
                        x++;if(x>=sx){x=0;z++;if(z>=sz){z=0;y++;}}
                    }
                }
            }
        }
        if(vd.length>0){
            nm=true;
            l=(max-mix+1);if((may-miy+1)>l)l=(may-miy+1);/*if((maz-miz+1)>l)l=(maz-miz+1);*/
            if(l)l=1.0/l;else l=0.5;
            for(i=0;i<vd.length;i+=4){
                var a=[];
                x=vd[i];y=vd[i+1];z=vd[i+2];n=vd[i+3];
                v.push(x*l);v.push(y*l);v.push(z*l);
                v.push(vt[n*4]);v.push(vt[n*4+1]);v.push(vt[n*4+2]);v.push(vt[n*4+3]);nv++;
                v.push((x+1)*l);v.push(y*l);v.push(z*l);
                v.push(vt[n*4]);v.push(vt[n*4+1]);v.push(vt[n*4+2]);v.push(vt[n*4+3]);nv++;
                v.push(x*l);v.push(y*l);v.push((z+1)*l);
                v.push(vt[n*4]);v.push(vt[n*4+1]);v.push(vt[n*4+2]);v.push(vt[n*4+3]);nv++;
                v.push((x+1)*l);v.push(y*l);v.push((z+1)*l);
                v.push(vt[n*4]);v.push(vt[n*4+1]);v.push(vt[n*4+2]);v.push(vt[n*4+3]);nv++;
                v.push(x*l);v.push((y+1)*l);v.push(z*l);
                v.push(vt[n*4]);v.push(vt[n*4+1]);v.push(vt[n*4+2]);v.push(vt[n*4+3]);nv++;
                v.push((x+1)*l);v.push((y+1)*l);v.push(z*l);
                v.push(vt[n*4]);v.push(vt[n*4+1]);v.push(vt[n*4+2]);v.push(vt[n*4+3]);nv++;
                v.push(x*l);v.push((y+1)*l);v.push((z+1)*l);
                v.push(vt[n*4]);v.push(vt[n*4+1]);v.push(vt[n*4+2]);v.push(vt[n*4+3]);nv++;
                v.push((x+1)*l);v.push((y+1)*l);v.push((z+1)*l);
                v.push(vt[n*4]);v.push(vt[n*4+1]);v.push(vt[n*4+2]);v.push(vt[n*4+3]);nv++;
                a=[];a.push(nv-8);a.push(-1);a.push(nv-6);a.push(-1);a.push(nv-7);a.push(-1);f.push(a);
                a=[];a.push(nv-7);a.push(-1);a.push(nv-6);a.push(-1);a.push(nv-5);a.push(-1);f.push(a);
                a=[];a.push(nv-8);a.push(-1);a.push(nv-4);a.push(-1);a.push(nv-6);a.push(-1);f.push(a);
                a=[];a.push(nv-4);a.push(-1);a.push(nv-2);a.push(-1);a.push(nv-6);a.push(-1);f.push(a);
                a=[];a.push(nv-5);a.push(-1);a.push(nv-2);a.push(-1);a.push(nv-6);a.push(-1);f.push(a);
                a=[];a.push(nv-2);a.push(-1);a.push(nv-5);a.push(-1);a.push(nv-1);a.push(-1);f.push(a);
                a=[];a.push(nv-8);a.push(-1);a.push(nv-4);a.push(-1);a.push(nv-7);a.push(-1);f.push(a);
                a=[];a.push(nv-4);a.push(-1);a.push(nv-3);a.push(-1);a.push(nv-7);a.push(-1);f.push(a);
                a=[];a.push(nv-4);a.push(-1);a.push(nv-2);a.push(-1);a.push(nv-3);a.push(-1);f.push(a);
                a=[];a.push(nv-2);a.push(-1);a.push(nv-1);a.push(-1);a.push(nv-3);a.push(-1);f.push(a);
                a=[];a.push(nv-7);a.push(-1);a.push(nv-3);a.push(-1);a.push(nv-5);a.push(-1);f.push(a);
                a=[];a.push(nv-3);a.push(-1);a.push(nv-1);a.push(-1);a.push(nv-5);a.push(-1);f.push(a);
            }
        }
        if(v.length>0){
            if(nm){
                var o=[],ax,ay,az,bx,by,bz,cx,cy,cz,d;
                for(i=0;i<f.length;i++){
                    ax=v[f[i][2]*7+0]-v[f[i][0]*7+0];
                    ay=v[f[i][2]*7+1]-v[f[i][0]*7+1];
                    az=v[f[i][2]*7+2]-v[f[i][0]*7+2];
                    bx=v[f[i][4]*7+0]-v[f[i][0]*7+0];
                    by=v[f[i][4]*7+1]-v[f[i][0]*7+1];
                    bz=v[f[i][4]*7+2]-v[f[i][0]*7+2];
                    cx=(ay*bz)-(az*by);
                    cy=(az*bx)-(ax*bz);
                    cz=(ax*by)-(ay*bx);
                    d=Math.sqrt(cx*cx+cy*cy+cz*cz);
                    if(d){cx/=d;cy/=d;cz/=d;}
                    o[i]=[cx,cy,cz];
                    f[i][1]=f[i][0]+nv;
                    f[i][3]=f[i][2]+nv;
                    f[i][5]=f[i][4]+nv;
                }
                for(i=0;i<nv*7;i++)v.push(0);
                for(i=0;i<f.length;i++){
                    for(j=0;j<3;j++){
                        v[f[i][j*2+1]*7+0]+=o[i][0];
                        v[f[i][j*2+1]*7+1]+=o[i][1];
                        v[f[i][j*2+1]*7+2]+=o[i][2];
                    }
                }
                for(i=nv;i<2*nv;i++){
                    d=Math.sqrt(v[i*7+0]*v[i*7+0]+v[i*7+1]*v[i*7+1]+v[i*7+2]*v[i*7+2]);
                    if(d){v[i*7+0]/=d;v[i*7+1]/=d;v[i*7+2]/=d;}
                }
            }
            canvas.i=f.length*3;
            m=f.length*3*10;
            var gl=canvas.gl,VBO=new Float32Array(m);
            for(i=n=0;i<f.length;i++){
                for(j=0;j<3;j++){
                    for(k=0;k<7;k++)
                        VBO[n++]=v[f[i][j*2]*7+k];
                    for(k=0;k<3;k++)
                        VBO[n++]=v[f[i][j*2+1]*7+k];
                }
            }
            canvas.v=gl.createBuffer();
            gl.bindBuffer(gl.ARRAY_BUFFER,canvas.v);
            gl.bufferData(gl.ARRAY_BUFFER,VBO,gl.STATIC_DRAW);
            canvas.c=gl.createBuffer();
            gl.bindBuffer(gl.ARRAY_BUFFER,canvas.c);
            gl.bufferData(gl.ARRAY_BUFFER,VBO,gl.STATIC_DRAW);
            canvas.n=gl.createBuffer();
            gl.bindBuffer(gl.ARRAY_BUFFER,canvas.n);
            gl.bufferData(gl.ARRAY_BUFFER,VBO,gl.STATIC_DRAW);
            canvas.p=m3d.perspective(45,gl.viewportWidth/gl.viewportHeight,0.1,100.0);
            canvas.m=m3d.identity();
            canvas.m[14]=-5.0;
            canvas.r=m3d.identity();
            canvas.d=false;
            canvas.addEventListener('mousedown',m3d.drag,true);
            canvas.addEventListener('touchstart',m3d.drag,true);
            canvas.addEventListener('mousemove',m3d.move,true);
            canvas.addEventListener('touchmove',m3d.move,true);
            canvas.addEventListener('mouseup', m3d.release,true);
            canvas.addEventListener('touchend', m3d.release,true);
            m3d.render(canvas);
            g.parentNode.replaceChild(canvas,g);
        }
    },
    /* polyfill, replace IMGs with CANVASes */
    init:function(){
        var i,t=document.querySelectorAll("img[src$=\".m3d\"]");
        try{
            for(i=0;i<t.length;i++){
                var canvas=document.createElement("CANVAS"),gl,r;
                canvas.setAttribute("width",t[i].getAttribute("width"));
                canvas.setAttribute("height",t[i].getAttribute("height"));
                canvas.setAttribute("id",t[i].id?t[i].id:"m3d_"+i);
                if(t[i].className)canvas.setAttribute("class",t[i].className);
                gl=canvas.gl=canvas.getContext("experimental-webgl")||canvas.getContext("webgl");
                gl.viewportWidth=canvas.width;
                gl.viewportHeight=canvas.height;
                gl.clearColor(0.0,0.0,0.0,0.0);
                gl.enable(gl.DEPTH_TEST);
                var vs=gl.createShader(gl.VERTEX_SHADER);
                var fs=gl.createShader(gl.FRAGMENT_SHADER);
                gl.shaderSource(vs,unescape(encodeURI(m3d.vs)));
                gl.compileShader(vs);
                if(!gl.getShaderParameter(vs,gl.COMPILE_STATUS))throw "vertex shader compilation failed";
                gl.shaderSource(fs, unescape(encodeURI(m3d.fs)));
                gl.compileShader(fs);
                if(!gl.getShaderParameter(fs,gl.COMPILE_STATUS))throw "fragment shader compilation failed";
                canvas.s=gl.createProgram();
                gl.attachShader(canvas.s,vs);
                gl.attachShader(canvas.s,fs);
                gl.linkProgram(canvas.s);
                if(!gl.getProgramParameter(canvas.s,gl.LINK_STATUS))throw "failed to link shader with gl context";
                canvas.s.pa=gl.getAttribLocation(canvas.s,"v");
                gl.enableVertexAttribArray(canvas.s.pa);
                canvas.s.na=gl.getAttribLocation(canvas.s,"n");
                gl.enableVertexAttribArray(canvas.s.na);
                canvas.s.ca=gl.getAttribLocation(canvas.s,"c");
                gl.enableVertexAttribArray(canvas.s.ca);
                canvas.s.pu=gl.getUniformLocation(canvas.s,"p");
                canvas.s.mu=gl.getUniformLocation(canvas.s,"m");
                r=new XMLHttpRequest();
                r.open("GET",t[i].src,true);
                r.overrideMimeType("application/octet-stream");
                r.responseType="arraybuffer";
                r.c=canvas; /* dirty hack, but (function(e, canvas){})(event, canvas); does not work on firefox */
                r.i=t[i];
                r.onload=function(e){if(this.readyState==4)m3d.load(this.c,this.i,this.response||this.mozResponseArrayBuffer);};
                r.send(null);
                t[i].src='data:image/gif;base64,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';
            }
        }catch(e){console.log("Unable to initialize WebGL:",e);}
    }
};
document.addEventListener("DOMContentLoaded",m3d.init);
