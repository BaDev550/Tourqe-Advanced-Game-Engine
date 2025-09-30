1. Temel Rendering Özellikleri
	## Shaders [Completed]
		1. Shader loading / Reloading
		2. Shader library
	## Buffers [Working-on]
		1. MultiAPI buffer layout (Buffer Element)
		2. Vertex Buffer Object, Element Buffer Object
		3. Vertex Array Object
		4. Framebuffer
	## Physically Based Rendering (PBR) [Not-Implemented]

	## HDR (High Dynamic Range) [Not-Implemented]
		1. Tonemapping (Reinhard, ACES)    [Not-Implemented]
		2. Gamma Correction                [Not-Implemented]

	---------------------------------------------------------
	## Material System [Not-Implemented]
		1. Multi-layered materials
		2. Texture maps: Albedo, Roughness, Metallic, Normal, AO, Height/Parallax, Emissive
	---------------------------------------------------------

	-------------------------------------------------------
	## Işıklandırma (Lighting) [Not-Implemented]
		1. Global Illumination (GI)
			Real-time voxel GI
			Screen-Space GI (SSGI)

		2. Direct Lighting
			Multiple light types: directional, point, spot, area lights
			Shadow casting per light type

	# Shadow Mapping [Not-Implemented]
		1. PCF (Percentage Closer Filtering)
		2. Variance shadow maps (VSM)
		3. Cascaded shadow maps (CSM) for directional lights

	# Image-Based Lighting (IBL) [Not-Implemented]
		1. Environment maps (cubemaps)
		2. Prefiltered specular maps (roughness levels)
		3. BRDF LUT convolution

	---------------------------------------------------------

	---------------------------------------------------------
	## Reflections & Refractions [Not-Implemented]
		1. Screen-Space Reflections (SSR)
		2. Refractions & transparency
	---------------------------------------------------------
 
	---------------------------------------------------------
	## Post-Processing [Not-Implemented]
		1. Bloom / Lens Flares
		2. Depth of Field
		3. Motion Blur
		4. Film Grain / Chromatic Aberration
		5. Color Grading
		6. LUT (Look-Up Tables)
		7. Exposure control
		8. SSAO
		9. Volumetric Effects
		10. Volumetric fog / smoke / light shafts
	---------------------------------------------------------

	---------------------------------------------------------
	## Optimization Techniques [Not-Implemented]
		1. Level of Detail (LOD)
		2. Geometry and textures
		3. Occlusion Culling
		4. Hierarchical Z-buffer / GPU culling
		5. Frustum Culling
		6. Batching & Instancing
		6. GPU instancing for repeated meshes
		7. Multi-threading / Job System
		8. Parallel rendering, asynchronous resource loading
		9. Deferred / Forward Rendering Hybrid
			Deferred for heavy scenes, forward for transparency + MSAA
	---------------------------------------------------------


	-- Sources
	https://chetanjags.wordpress.com/category/blackmesa-source-engine
	https://learnopengl.com/PBR/Lighting
	https://learnopengl.com/Advanced-Lighting/Bloom
	https://learnopengl.com/Advanced-Lighting/SSAO
	https://www.ogldev.org/www/tutorial35/tutorial35.html
	https://www.ogldev.org/www/tutorial43/tutorial43.html
	https://www.ogldev.org/www/tutorial49/tutorial49.html