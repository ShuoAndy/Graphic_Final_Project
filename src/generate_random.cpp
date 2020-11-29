#include <iostream>
#include <cstdio>
#include <cmath>
using namespace std;

int main(){
    int total_num = 22 * 22;
    int num_material = 2*total_num + 4;
    cout << "Materials {\n";
    cout << "numMaterials " << num_material << endl;
    cout << "DiffuseMaterial { attenuation 0.5 0.5 0.5 }\n";
    cout << "DiffuseMaterial { attenuation 0.4 0.2 0.1 }\n";
    cout << "MetalMaterial { attenuation 0.7 0.6 0.5 \nfuzz 0.0\n}\n";
    cout << "DielecMaterial { refractive 1.5 }\n";
    for (int i = 0; i < total_num; i ++){
        cout << "DiffuseMaterial { " << "attenuation " << drand48() * drand48() << " " << drand48() * drand48() << " " << drand48() * drand48() << " }\n";
    }
    for (int i = 0; i < total_num; i ++){
        cout << "MetalMaterial { " << "attenuation " << 0.5 * (drand48() + 1) << " " << 0.5 * (drand48() + 1) << " " << 0.5 * (drand48() + 1) << "\n" << "fuzz " << 0.5*drand48() << " }\n";
    }
    cout << "}\n";
    cout << "Group {\n";
    cout << "numObjects " << total_num + 4 << endl;
    cout << "MaterialIndex " << 0 << endl;
    cout << "Sphere {\n center 0 -1000 0\n radius 1000\n}\n";
    cout << "MaterialIndex " << 3 << endl;
    cout << "Sphere {\n center 0 1 0\n radius 1.0\n}\n";
    cout << "MaterialIndex " << 1 << endl;
    cout << "Sphere {\n center -4 1 0\n radius 1.0\n}\n";
    cout << "MaterialIndex " << 2 << endl;
    cout << "Sphere {\n center 4 1 0\n radius 1.0\n}\n";
    int cnt = 4;
    for (int a = -11; a < 11; a += 1)
        for (int b = -11; b < 11; b += 1){
            float prob = drand48();
            float x = a + 0.9 * drand48(), y = 0.2, z = b + 0.9 * drand48();
            while(powf(x - 4.0, 2) + powf(z, 2) <= 0.81){
                x = a + 0.9 * drand48(), y = 0.2, z = b + 0.9 * drand48();
            }
            if (prob < 0.8){
                cout << "MaterialIndex " << cnt << endl;
            } else if (prob < 0.95){
                cout << "MaterialIndex " << total_num + cnt << endl;
            } else {
                cout << "MaterialIndex 3" << endl;
            }
            cout << "Sphere {\n center " << x << " " << y << " " << z << "\n radius 0.2\n}\n";
            cnt ++;
        }
    cout << "}\n";
    return 0;
}