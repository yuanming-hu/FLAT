simple:
	g++ *.cpp -o physics -I ~/AntTweakBar/include -L ~/AntTweakBar/lib -lAntTweakBar -w -fpermissive -lglfw -lm -framework OpenGL
#g++ *.cpp -o physics -I ~/AntTweakBar/include -L ~/AntTweakBar/lib -lAntTweakBar -lX11 -lGL -w -fpermissive -lglfw -lXrandr  -lrt -lX11 -lrt -lGL -pthread -lm

run:
	LD_PRELOAD=./libAntTweakBar.so ./physics
