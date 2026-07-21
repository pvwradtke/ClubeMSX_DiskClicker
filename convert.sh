../MSXgl/tools/MSXtk/bin/MSXimg sprites.png -mode sprt -size 16 16 -num 6 1 -format c -name sprites -out sprites.h
../MSXgl/tools/MSXtk/bin/MSXimg tiles.png -format c -mode gm2 -size 264 64 -num 32 8 --tilesUnique --noTilesName -name tiles
../MSXgl/tools/MSXtk/bin/MSXbin musica.akg -at 0xA000
../MSXgl/tools/MSXtk/bin/MSXbin efeitos.akx -at 0x9000