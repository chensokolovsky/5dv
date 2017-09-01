VPATH = src

5dv:
	gcc -framework SDL2 `sdl-config --libs` -o 5dv ./src/main.c ./src/DCT.c ./src/EncDecChromaBlocks.c ./src/EncDecDCT.c ./src/EncDecMacroBlocks.c ./src/EncDecQuantization.c ./src/EncDecZigZag.c ./src/fastDCT.c ./src/FDVFile.c ./src/FiveDVEncDec.c ./src/FiveDVFrameEncDec.c ./src/FiveDVUFile.c ./src/MovieFrameGrabber.c ./src/Sim3D2DConverter.c ./src/SimShapeCreator.c ./src/SpaceShapesCreator.c ./src/ZigZag.c ./src/zips.c -F/Library/Frameworks