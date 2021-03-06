#version 330 core

#pragma tdm_include "stages/interaction/interaction.params.glsl"

in vec4 attr_Position;
in vec4 attr_TexCoord;
in vec3 attr_Tangent;
in vec3 attr_Bitangent;
in vec3 attr_Normal;
in vec4 attr_Color;
in int attr_DrawId;
 
out vec3 var_Position;
out vec2 var_TexDiffuse;
out vec2 var_TexSpecular;
out vec2 var_TexNormal;
out vec4 var_TexLight;
out mat3 var_TangentBinormalNormalMatrix;
out vec4 var_Color;
out vec3 var_tc0;
out vec3 var_localViewDir;
out vec4 var_ClipPosition;
flat out int var_DrawId;

void main( void ) {     
	// transform vertex position into homogenous clip-space  
	var_ClipPosition = u_projectionMatrix * (params[attr_DrawId].modelViewMatrix * attr_Position);
	gl_Position = var_ClipPosition;
	
	// transform vertex position into world space  
	var_Position = attr_Position.xyz;

	// normal map texgen   
	var_TexNormal.x = dot(attr_TexCoord, params[attr_DrawId].bumpMatrix[0]);
	var_TexNormal.y = dot(attr_TexCoord, params[attr_DrawId].bumpMatrix[1]);
 
	// diffuse map texgen      
	var_TexDiffuse.x = dot(attr_TexCoord, params[attr_DrawId].diffuseMatrix[0]);
	var_TexDiffuse.y = dot(attr_TexCoord, params[attr_DrawId].diffuseMatrix[1]);
 
	// specular map texgen  
	var_TexSpecular.x = dot(attr_TexCoord, params[attr_DrawId].specularMatrix[0]);
	var_TexSpecular.y = dot(attr_TexCoord, params[attr_DrawId].specularMatrix[1]);
 
	// light projection texgen
	var_TexLight = (attr_Position * params[attr_DrawId].lightProjectionFalloff).xywz;

	// construct tangent-binormal-normal 3x3 matrix    
	var_TangentBinormalNormalMatrix = mat3( attr_Tangent, attr_Bitangent, attr_Normal ); 
	//var_tc0 = u_lightOrigin.xyz.xyz * var_TangentBinormalNormalMatrix;
	var_localViewDir = (params[attr_DrawId].viewOrigin.xyz - var_Position).xyz;

	// primary color 
	var_Color = (attr_Color * params[attr_DrawId].colorModulate) + params[attr_DrawId].colorAdd;  
	
	var_DrawId = attr_DrawId;
}