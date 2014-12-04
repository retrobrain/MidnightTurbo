#ifdef GL_ES
precision mediump float;
#endif

varying vec4 v_fragmentColor;	
varying vec2 v_texCoord;

uniform vec2 u_blurCenter;
uniform float u_blurSize;
uniform float u_time;
		
void main()			
{
    vec2 texCoord = v_texCoord;

    vec2 samplingOffset = 1.0/100.0 * (u_blurCenter - texCoord) * (u_blurSize * 0.015) ;
    vec4 fragmentColor = texture2D(CC_Texture0, texCoord) * 0.18;

    fragmentColor += texture2D(CC_Texture0, texCoord + samplingOffset) * 0.15;
    fragmentColor += texture2D(CC_Texture0, texCoord + (2.0 * samplingOffset)) *  0.12;
    fragmentColor += texture2D(CC_Texture0, texCoord + (3.0 * samplingOffset)) * 0.09;
    fragmentColor += texture2D(CC_Texture0, texCoord + (4.0 * samplingOffset)) * 0.05;
    fragmentColor += texture2D(CC_Texture0, texCoord - samplingOffset) * 0.15;
    fragmentColor += texture2D(CC_Texture0, texCoord - (2.0 * samplingOffset)) *  0.12;
    fragmentColor += texture2D(CC_Texture0, texCoord - (3.0 * samplingOffset)) * 0.09;
    fragmentColor += texture2D(CC_Texture0, texCoord - (4.0 * samplingOffset)) * 0.05;

    fragmentColor *= 0.9+0.07*sin(10.0*u_time-texCoord.y*800.0);

    gl_FragColor = fragmentColor;
}
