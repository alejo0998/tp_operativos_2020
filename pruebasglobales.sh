cd cliente
mkdir bin
make

cd ..
cd app
mkdir bin
make

cd ..
cd restaurante
mkdir bin
make

cd ..
cd sindicato
mkdir bin
make

cd ..
cd comanda
mkdir bin
make

cd ..

rm -f -r ../restaurante1
cp -r restaurante ../restaurante1
cp ../restaurante1/cfg/restaurante1.config ../restaurante1/cfg/restaurante.config 

rm -f -r ../restaurante2
cp -r restaurante ../restaurante2
cp ../restaurante2/cfg/restaurante2.config ../restaurante2/cfg/restaurante.config 

rm -f -r ../restaurante3
cp -r restaurante ../restaurante3
cp ../restaurante3/cfg/restaurante3.config ../restaurante3/cfg/restaurante.config 

rm -f -r ../cliente1
cp -r cliente ../cliente1
cp ../cliente1/cfg/cliente1.config ../cliente1/cfg/cliente.config 

sudo rm -f -r ../cliente2
cp -r cliente ../cliente2
cp ../cliente2/cfg/cliente2.config ../cliente2/cfg/cliente.config 

rm -f -r ../cliente3
cp -r cliente ../cliente3
cp ../cliente3/cfg/cliente3.config ../cliente3/cfg/cliente.config 

