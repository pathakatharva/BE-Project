#version 450 core
		in vec2 textureCoord_out;
		in vec3 toLightVector;
		in vec3 toCameraVector;
		in vec3 surfaceNormal;

		out vec4 FragColor;

		uniform vec3 uKd;
		uniform vec3 uLightColor;

		uniform sampler2D tSampler;
		uniform sampler2D normSampler;
		uniform float uShineDamper;
		uniform float uReflectivity;
		

		uniform int Lkey;

		void main(void)
		{

		vec3 xNormal = normalize(surfaceNormal);
		vec3 zToLightVector = normalize(toLightVector);
		vec3 zToCameraVector = normalize(toCameraVector);

		//DIFFUSE
		float NdotL = dot(xNormal,zToLightVector);
		float brightness = max(NdotL,0.1);
		vec3 diffuse_light = vec3( uLightColor  * brightness*uKd) ;
		vec4 fTexture = texture(tSampler  ,textureCoord_out,-0.2);
		
		//vec4 color= vec4(1.0,1.0,1.0,1.0);

		//SPECULAR 
		vec3 zLightDirection = -zToLightVector;
		vec3 reflectedLightDirection = reflect(zLightDirection,xNormal);
		float specularFactor =max( dot(reflectedLightDirection,zToCameraVector),0.0);
		float dampedFactor = pow(specularFactor,uShineDamper);
		vec3 specular_light = dampedFactor * uLightColor  * uReflectivity;

		vec4 color = vec4(diffuse_light,1.0)+ vec4(specular_light,1.0) ;

			if(Lkey==1)	
		FragColor = 1.2 * color *fTexture  ;
		else
		FragColor = vec4(1,1,1,1);

	
	
	}
