#ifndef Matrix_h
#define Matrix_h
#include <vector>
using namespace std;

namespace FinalProj{
    
    template<class T>
    class Matrix{
    public:
        Matrix(){};
        Matrix(int h, int w): height(h), width(w), mat(3,vector(4,T())){} // T initialization T()
        vector<T>& operator[](int index){ return mat[index]; }
        void clear(){
            mat.clear();
            height = 0;
            width = 0;
        }
        
    private:
        vector<vector<T>> mat; 
        int height;
        int width;
        
    };
}

#endif /* Matrix_h */
