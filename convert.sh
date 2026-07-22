../MSXgl/tools/MSXtk/bin/MSXimg sprites.png -mode sprt -size 16 16 -num 7 1 -format c -name sprites -out sprites.h
../MSXgl/tools/MSXtk/bin/MSXimg tiles.png -mode gm2 -size 256 64 -num 32 8 --tilesUnique --noTilesName -format c -name tiles -out tiles.h
../MSXgl/tools/MSXtk/bin/MSXbin musica.akg -at 0xA000
../MSXgl/tools/MSXtk/bin/MSXbin efeitos.akx -at 0x9000
