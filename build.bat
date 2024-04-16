:: Crează un director pentru build-uri
mkdir build
cd build
cmake ../
cmake --build . --config Debug
cmake --build . --config Release
cmake --install . --config Release
cd ..
pause
