#include <iostream>

using namespace std;

int board[10][10]; //0 for unknown, 1 for hit, 2 for miss, 3 for sunk
//First go for things with highest probability of hitting than with the most mutual information about other squares
//Once we know the information, use mutual information to update
float prob[10][10]; //ESTIMATED probability of having a hit/miss, given by the surrounding coordinates

void probabilityReestimate()
{   
    //Estimates probabilities
    float newprob[10][10];
    //Looks through all points in board
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            newprob[i][j]=0;
            //If coordinate has not been hit, missed, or sunk
            if(board[i][j]==0)
            {
                //Looks at all adjacent neighbors (vertically or horizontally)
                for(int k=-4;k<=4;k++)
                {
                    if(k!=0)
                    {
                        //Weights of 10 for closest neighbors, 6 for neighbors 2 away, 3 for neighbors 3 away, 1 for neighbors 4 away
                        float coeff[4]={10.0,6.0,3.0,1.0};
                        //Checks neighbor coordinate is valid
                        if(i+k<10&&i+k>=0)
                        {
                            newprob[i][j]+=prob[i+k][j]*coeff[abs(k)-1];
                        }
                        if(j+k<10&&j+k>=0)
                        {
                            newprob[i][j]+=prob[i][j+k]*coeff[abs(k)-1];
                        }
                    }
                }
            }
            //If coordinate has been hit (but not sunk), probavility is 1
            else if(board[i][j]==1)
            {
                newprob[i][j]=1;
            }
        }
    }
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            //Keeps probabilities reasonable and less than 1
            if(board[i][j]!=1)
            prob[i][j]=newprob[i][j]/80;
        }
    }
}
void updateSunk(int x, int y)
{
    //Updates if a ship has been sunk by looking in all directions
    int xdir[4]={-1,1,0,0},ydir[4]={0,0,-1,1};
    for(int i=0;i<4;i++)
    {
        if(x+xdir[i]>=0 && x+xdir[i]<10 && y+ydir[i]>=0 && y+ydir[i]<10)
        {
            if(board[x+xdir[i]][y+ydir[i]]==1)
            {
                for(int j=0;j<5;j++)
                {
                    int newx = x+j*xdir[i], newy = y+j*ydir[i];
                    if(board[newx][newy] !=1)
                    {
                        break;
                    }
                    board[newx][newy] = 3;
                    prob[newx][newy] = 0;
                }
                break;
            }
        }
    }
    //Recheck in edge case 
    board[x][y] = 3;
    prob[x][y] = 0;
}
pair<int, int> chooseBestMove()
{
    //Choose the best move as the coordinate with best probability
    float bestProb=0;
    pair<int, int> bestCoord;
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            if(board[i][j]==0&&prob[i][j]>bestProb)
            {
                bestProb=prob[i][j];
                bestCoord.first=i;
                bestCoord.second=j;
            }
        }
    }
    return bestCoord;
}
void printBoard()
{
    //Prints out the board
    cout<<"    ";
    for(int i=0;i<10;i++)
    {
        cout<<i+1<<" ";
    }
    cout<<endl;
    for(int i=0;i<10;i++)
    {
        cout<<10-i<<"  ";
        if(i!=0)
        {
            cout<<" ";
        }
        for(int j=0;j<10;j++)
        {
            if(board[i][j]==0)
            {
                cout<<'.';
            }
            if(board[i][j]==1)
            {
                cout<<'X';
            }
            if(board[i][j]==2)
            {
                cout<<'M';
            }
            if(board[i][j]==3)
            {
                cout<<'S';
            }
            cout<<" ";
        }
        cout<<endl;
    }
}
int main()
{
    long long numMoves=0, correctCounter=0;
    // Initialize probability matrix using combinatorics
    for(int i=0;i<10;i++)
    {
        for(int j=0;j<10;j++)
        {
            long long xdist = min(i+1,10-i);
            long long ydist = min(j+1,10-j);
            long long mindisttoedge = min(xdist,ydist);
            long long combinations = mindisttoedge*(mindisttoedge+1);
            prob[i][j]=combinations*1.0/100;
        }
    }
    /*
    for(int i=0;i<10;i++)
        {
            for(int j=0;j<10;j++)
            {
                cout<<prob[i][j]<<" ";
            }
            cout<<endl;
        }
    */
   //Runs the game 
    while(true)
    {
        printBoard();
        cout<<"Number of moves currently: "<<numMoves<<endl;
        cout<<"What coordinate do you want to try?"<<endl;
        pair<int, int> move;
        char l;
        move = chooseBestMove();
        long long x = move.first;
        long long y= move.second;
        cout<<"Computer tries Row:"<<10-x<<" Col:"<<y+1<<endl;
        cout<<"Was that a hit (H), miss (M), or sunk (S)?"<<endl;
        cin>>l;
        //Sets value of coordinate based on result of query
        if (l=='H')
        {
            board[x][y]=1;
            prob[x][y]=1;
            correctCounter++;
        }
        if (l=='M')
        {
            board[x][y]=2;
            prob[x][y]=0;
        }
        if (l=='S')
        {
            board[x][y]=1;
            updateSunk(x,y);
            correctCounter++;
        }
        numMoves+=1;
        //Ends game
        if(correctCounter==17)
        {
            break;
        }
        probabilityReestimate();
        /*
        for(int i=0;i<10;i++)
        {
            for(int j=0;j<10;j++)
            {
                cout<<prob[i][j]<<" ";
            }
            cout<<endl;
        }
        */
    }
    printBoard();
    cout<<"Number of moves when game ends: "<<numMoves<<endl;
}