@echo off
glslandValidator -V light_vertex.glsl -o light_vertex.glsl.spv
glslandValidator -V light_fragment.glsl -o light_fragment.glsl.spv
pause