// Fragment Shader

static const char* PhongFSH = STRINGIFY
(
 
// Varying
varying highp vec3 vNormal;
varying highp vec2 vTexel;
 
// Uniforms
uniform highp vec3 uDiffuse;
uniform highp vec3 uSpecular;
uniform sampler2D uTexture;
 
void main(void)
{
    // Material
    highp vec3 ka = vec3(0.05);
    highp vec3 kd = uDiffuse;
    highp vec3 ks = uSpecular;
    highp float alpha = 1.0;
    
    // Light
    highp vec3 ia = vec3(1.0);
    highp vec3 id = vec3(1.0);
    highp vec3 is = vec3(1.0);
    
    // Vectors
    highp vec3 L = normalize(vec3(1.0, 1.0, 1.0));
    highp vec3 N = normalize(vNormal);
    highp vec3 V = normalize(vec3(0.0, 0.0, 1.0));
    highp vec3 R = reflect(L, N);
    
    // Illumination factors
    highp float df = min(1.0,max(0.0, dot(L, N)*1.1));
    highp float sf = pow(max(0.0, dot(R, V)), alpha);
    
    // Phong reflection equation
    highp vec3 Ip = ka*ia + kd*id*df + ks*is*sf;
    
    highp vec2 texel=vTexel;
    // Decal
    while(texel.x<0.0)texel.x =texel.x+1.0;
    while(texel.x>1.0)texel.x =texel.x-1.0;
    while(texel.y<0.0)texel.y =texel.y+1.0;
    while(texel.y>1.0)texel.y =texel.y-1.0;
    highp vec4 decal = texture2D(uTexture, texel);
    
    // Surface
    highp vec3 surface;
    if(decal.a > 0.0)
        surface = decal.rgb*df + decal.rgb*sf;
    else
        surface = Ip;
    
    gl_FragColor = vec4(surface, 1.0);
}
 
);
