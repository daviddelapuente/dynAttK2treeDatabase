//receive a morton path, return the node were the edge begin
int a(uint8_t path[],int pathLength){
    int ri=0;
    int rf=pow(2,pathLength)-1;

    int i=0;
    int medio;
    while(ri<rf){
        medio=(ri+rf)/2;
        if(path[i]<=1){
            rf=medio;
        }else{
            ri=medio +1;
        }
        i++;
    }

    return ri;
}

//receive a morton path, return the node were the edge ends
int b(uint8_t path[],int pathLength){

    int ri=0;
    int rf=pow(2,pathLength)-1;;

    int i=0;
    int medio;
    while(ri<rf){
        medio=(ri+rf)/2;
        if(path[i]%2==0){
            rf=medio;
        }else{
            ri=medio +1;
        }
        i++;
    }
    return ri;
}

//receive a morton path, return the edge in the form of (a,b)
int * ab(uint8_t path[],int pathLength){
    static int node[2];
    node[0]=a(path,pathLength);
    node[1]=b(path,pathLength);
    return node;
}

//receive a node (a,b) return the morton code;
uint8_t * morton(int a,int b, int pathLength){
    static uint8_t path[30];
    int ri=0;
    int rf=pow(2,pathLength)-1;

    int ci=0;
    int cf=pow(2,pathLength)-1;

    int i=0;
    int rmedio;
    int cmedio;
    while(ri<rf){
        rmedio=(ri+rf)/2;
        if(b<=rmedio){
            //path[i] can be 0 or 2
            cmedio=(ci+cf)/2;
            if(a<=cmedio){
                path[i]=0;
                cf=cmedio;
            }else{
                path[i]=2;
                ci=cmedio+1;
            }
            rf=rmedio;
        }else{
            //path[i] can be 1 or 3
            cmedio=(ci+cf)/2;
            if(a<=cmedio){
                path[i]=1;
                cf=cmedio;
            }else{
                path[i]=3;
                ci=cmedio+1;
            }
            ri=rmedio +1;
        }
        i++;
    }
    return path;
}
