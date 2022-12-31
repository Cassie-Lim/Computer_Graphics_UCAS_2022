cd *$1* && \
g++ main.cpp -lglut -lGLU -lGL -o main && \
cd .. && \
echo "Compile exp$1 finsihed!"
