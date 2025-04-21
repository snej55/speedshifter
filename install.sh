cmake -B build/ --install-prefix ~/local
cmake --build build/
echo "Built targets!"
sudo cmake --install build/
echo "Installed!"
