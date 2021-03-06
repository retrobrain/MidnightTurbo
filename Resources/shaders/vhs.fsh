#ifdef GL_ES
precision mediump float;
#endif

uniform vec2 u_resolution;
uniform float u_time;
varying vec2 v_texCoord;

vec2 screenDistort(vec2 uv)
{
    uv -= vec2(.5,.5);
    uv = uv*1.2*(1./1.2+2.*uv.x*uv.x*uv.y*uv.y*2.0);
    uv += vec2(.5,.5);
    return uv;
}

vec2 scandistort(vec2 uv) 
{
    float amplitude = 10.0;
    float scan1 = clamp(cos(uv.y * 2.0 + u_time), 0.0, 1.0);
    float scan2 = clamp(cos(uv.y * 2.0 + u_time + 4.0) * amplitude, 0.0, 1.0) ;
    float amount = scan1 * scan2 * uv.x;

    uv.x -= 0.03 * amount;

    return uv;}

vec4 noise(){
    vec2 uv = gl_FragCoord.xy / sin(u_resolution.xy * u_time * 0.01);

    vec2 p = sin(uv * 13.0 + uv.x * u_time * sin(uv.y));

    vec4 noi = vec4(vec3(p.x * p.y), 1.);
    return noi;}

void main(void){
    vec2 uv = v_texCoord;

    uv = scandistort(uv);	
    uv = screenDistort(uv);
    
    //rgb offset
    float offset = 0.0;
    offset = (1.0 + cos(u_time*2.0)) * 0.15;
    offset *= 1.0 + cos(u_time*40.0) * 0.2;
    offset = pow(offset, 2.0);
    offset *= 0.03; 
    vec3 col;
    col.r = texture2D(CC_Texture0,vec2(uv.x+offset,uv.y)).r;
    col.g = texture2D(CC_Texture0,vec2(uv.x,uv.y+offset*2.)).g;
    col.b = texture2D(CC_Texture0,vec2(uv.x-offset,uv.y)).b;
    
    //scanlines
    col *= 0.9+0.1*sin(20.0*u_time-uv.y*800.0);

    // phosphor vibrance
    col *= 0.97+0.02*sin(110.0*u_time);

    gl_FragColor = vec4(col,1.0);}


