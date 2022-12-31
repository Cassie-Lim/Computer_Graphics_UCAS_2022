cd *$1* && \
g++ main.cpp -lglut -lGLU -lGL -o main && \
echo "Compile exp$1 finsihed!"
echo "Loading window for exp$1..."
./main
