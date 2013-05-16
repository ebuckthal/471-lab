uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uColor;
uniform vec3 uAmbient;
uniform vec3 uSpecular;
uniform vec3 uDiffuse;
uniform float uShininess;

attribute vec3 aPosition;
attribute vec3 aNormal;
//add a normal attribute

varying vec3 vColor;

void main() {
  vec4 vPosition;
 
  /* First model transforms */
  vPosition = uModelMatrix* vec4(aPosition.x, aPosition.y, aPosition.z, 1);
  vPosition = uViewMatrix* vPosition;
  gl_Position = uProjMatrix*vPosition;
  
  vec3 v = vec3((uModelMatrix*uViewMatrix*vec4(aPosition, 1)).xyz);
  vec3 n = normalize(vec3((uModelMatrix*uViewMatrix*vec4(normalize(aNormal), 0)).xyz));
  
  vec3 lightPos = vec3(0,5,-3);
  vec3 l = normalize(lightPos-v);
  vec3 e = normalize(-v);
  vec3 r = normalize(2.0*(dot(l,n))*n - l);

  float d = max(0.0, dot(n,l));
  float s = pow(max(0.0, dot(r,e)), uShininess);

  vColor = d*uDiffuse;
}
