#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <dirent.h>
#include <filesystem>
using namespace std;
int wordnumber;
int wordsize[24];
int totallength=0;
long long totalfrequency=0;
int totalmoves=0, incorrectmoves=0, blanksknown=0;
bool lieDetected=false;
char words[24][12]; //'0' is blank
int viable_letter[26]; //0 is ok, 1 is guessed correctly, 2 is guessed incorrectly, 3 is guessed incorrectly twice
map<char,int> letter_index;
map<int,char> number_index;
map<string,float> word_freq;
set<string> guessedPhrases;
map<int,pair<int,int> > positiontoword;
set<string> allwords;
bool makeGuess = false;
string path = "word_freq.csv";

//ifstream fin2("/usr/share/dict/words")
void read_database()
{
    ifstream fin(path);
    string line;
    /*
    while(!fin2.eof())
    {
        getline(fin2,line);
        allwords.insert(line);
    }
    */
    //Gets words and frequencies from database
    if(!fin.is_open())
    {
        cout << "Unable to read database" << endl;
    }
    getline(fin,line);
    while(!fin.eof())
    {
        getline(fin,line);
        string word = line.substr(0,line.find(','));
        string freq = line.substr(line.find(',')+1,line.length()-line.find(',')-1);
        if(freq.length()>3)
        {
            float frequency=stof(freq);
            word_freq[word]=frequency;
            totalfrequency+=frequency;
        }
    }
    //Converts frequency total into a float decimal
    for(auto it=word_freq.begin();it!=word_freq.end();++it)
    {
        it->second=it->second/totalfrequency;
        //cout<<it->first<<" "<<it->second<<endl;
    }

}
void initialize()
{
    cout<<"Please input the full path to your 'word_freq.csv' file"<<endl;
    cin>>path;
    //Initializes game by asking lengths of words
    cout<<"How many words are in the phrase?"<<endl;
    cin>>wordnumber;
    for(int i=0;i<wordnumber;i++)
    {
        cout<<"How many letters in word "<<i+1<<"?"<<endl;
        cin>>wordsize[i];
        totallength+=wordsize[i];
    }
    for(int i=0;i<wordnumber;i++)
    {
        for(int j=0;j<wordsize[i];j++)
        {
            words[i][j]='0';
        }
    }
    char currentChar='a';
    //Assigns easy-access indexes for each letter 'a' - 'z'
    for(int i=0;i<26;i++)
    {
        letter_index[currentChar]=i;
        number_index[i]=currentChar;
        currentChar++;
    }
}
void printBoard()
{
    //Prints the board in a suitable way
    int curr_position=0;
    for(int i=0;i<wordnumber;i++)
    {
        for(int j=0;j<wordsize[i];j++)
        {
            if (words[i][j]=='0')
            {
                cout<<"_ ";
            }
            else
            {
                cout<<words[i][j]<<" ";
            }
            //Assigns a position within the phrase to a spot within a word
            positiontoword[curr_position]=make_pair(i,j);
            curr_position++;
        }
        cout<<" ";
        positiontoword[curr_position]=make_pair(i,-1);
        curr_position++;
    }
    cout<<endl;
}
float calculate_expected_word (char a, int index)
{
    // Calculates the expected frequency for character for words in the database that
    // could fit within a word in the phrase
    float expected = 0;
    //Looks through all words in dictionary
    for(auto it=word_freq.begin();it!=word_freq.end();++it)
    {
        string word = it->first;
        float freq=it->second;
        bool ok=true;
        //Word lengths have to match
        if(word.length()!=wordsize[index])
        {
            continue;
        }
        //Incorrectly guessed letters cannot appear in a word
        for(int i=0;i<26;i++)
        {
            if(viable_letter[i]==2||viable_letter[i]==3)
            {
                if(word.find(number_index[i])!=-1)
                {
                    ok=false;
                }
            }
        }
        if(!ok)
        {
            continue;
        }
        int number_appearances=0;
        for(int i=0;i<word.length();i++)
        {
            //If we know a character at a location in the word, the word in the database has to have 
            //the same character at that location
            if(words[index][i]!='0'&&words[index][i]!=word[i])
            {
                ok=false;
                continue;
            }
            //If we know a character is in the word, but the word in the database contains that character
            //at a different position than specified, it is not ok
            if(words[index][i]=='0'&&viable_letter[letter_index[word[i]]]==1)
            {
                ok=false;
                continue;
            }
            if(word[i]==a)
            {
                number_appearances+=1;
            }
        }
        if(ok)
        {
            //Calculates expected frequency by adding number of appearances*word frequency
            expected+=number_appearances*freq;
        }
    }
    return expected;
}
float calculate_expected_phrase (char a)
{
    //Collates a frequency for one character by adding up all expected frequencies for each word
    float expected = 0;
    for(int i=0;i<wordnumber;i++)
    {
        expected+=calculate_expected_word(a, i);
    }
    return expected;
}
float weight()
{
    //weight function
    float inter=0.005*blanksknown/totallength;
    return 0.0006*incorrectmoves*incorrectmoves+inter;
}
char getBestChar()
{
    //Gets best character
    char currentChar='a';
    float best_freq=0;
    float best_char;
    for(int i=0;i<26;i++)
    {
        //Character must not be guessed correctly or guessed incorrextly twice
        if(viable_letter[i]!=1&&viable_letter[i]!=3)
        {
            bool lie_check=false;
            //If character only guessed incorrectly once
            if(viable_letter[i]==2)
            {
                lie_check=true;
                viable_letter[i]=0;
            }
            float frequency=calculate_expected_phrase(currentChar);
            //Finds frequency of character
            if(lie_check)
            {
                viable_letter[i]=2;
            }
            //If character has not been guessed and frequency > best frequency
            //Guess that character
            if(viable_letter[i]==0&&frequency>best_freq)
            {
                best_freq=frequency;
                best_char=currentChar;
            }
            //If character has been guessed incorrectly and frequency > weight()*best frequency
            // Guess that character
            if(!lieDetected&&viable_letter[i]==2&&weight()*frequency>best_freq)
            {
                best_freq=weight()*frequency;
                best_char=currentChar;
            }
            //cout<<currentChar<<" "<<frequency<<endl;
        }
        currentChar++;
    }
    return best_char;
}
string makeAGuess()
{
    //Has to know there is a lie to make a guess
    if(!lieDetected)
    {
        return "";
    }
    //Constant to make a guess
    int k = 1000;
    bool ok = true;
    string ans = "";
    //Goes through every word
    for (int i=0;i<wordnumber;i++)
    {
        float bestfrequency=0, secondfrequency=0;
        string bestword;
        //Finds the two best frequencies of words that satisfy the constraints of word i in the phrase
        for(auto it=word_freq.begin();it!=word_freq.end();++it)
        {
            string word = it->first;
            float freq=it->second;
            bool ok=true;
            if(word.length()!=wordsize[i])
            {
                continue;
            }
            for(int j=0;j<26;j++)
            {
                if(viable_letter[j]==2||viable_letter[j]==3)
                {
                    if(word.find(number_index[j])!=-1)
                    {
                        ok=false;
                    }
                }
            }
            if(!ok)
            {
                continue;
            }
            for(int j=0;j<word.length();j++)
            {
                if(words[i][j]!='0'&&words[i][j]!=word[j])
                {
                    ok=false;
                    continue;
                }
                if(words[i][j]=='0'&&viable_letter[letter_index[word[j]]]==1)
                {
                    ok=false;
                    continue;
                }
            }
            if(ok)
            {
                //Correctly changes best frequency and second best frequency
                if(freq>bestfrequency)
                {
                    secondfrequency=bestfrequency;
                    bestfrequency=freq;
                    bestword=word;
                    //cout<<word<<" "<<freq<<endl;
                }
                else if(freq>secondfrequency)
                {
                    secondfrequency=freq;
                }
            }
        }
        //Second frequency is not unlikely enough to be able to guess
        if(bestfrequency<secondfrequency*k)
        {
            return "";
        }
        else
        {
            //Second frequency is unlikely enough that we can guess the whole word
            //This deals with cases where only one word is possible since then
            //second best frequency will be 0
            ans += bestword;
            ans += " ";
        }
    }
    //If all conditions satisfied for all words in phrase and we have not incorrectly
    //guessed this phrase already, we can guess the phrase
    if(guessedPhrases.find(ans)==guessedPhrases.end())
    {
        return ans;
    }
    return "";
}
int main()
{
    initialize();
    read_database();
    //Sets and plays the game
    while(true)
    {
        bool done = true;
        for(int i=0;i<wordnumber;i++)
        {
            for(int j=0;j<wordsize[i];j++)
            {
                if(words[i][j]=='0')
                {
                    done = false;
                }
            }
        }
        if(done)
        {
            break;
        }
        cout<<"Number of Questions Asked: "<<totalmoves<<endl;
        printBoard();
        string guess = makeAGuess();
        //If we are to ask for a character
        if(guess=="")
        {
            char guessedChar=getBestChar();
            cout<<"Is \'"<<guessedChar<<"\' in the phrase? (Y) or (N)"<<endl;
            char ind;
            cin>>ind;
            if(ind=='N')
            {
                //Changes letters to be used
                int index=letter_index[guessedChar];
                if(viable_letter[index]==0)
                {
                    viable_letter[index]=2;
                }
                else if(viable_letter[index]==2)
                {
                    viable_letter[index]=3;
                }
                incorrectmoves++;
            }
            if(ind=='Y')
            {
                cout<<"How many positions is the letter in?"<<endl;
                int siz;
                cin>>siz;
                blanksknown+=siz;
                cout<<"Where are the positions? (Input one number at a time)"<<endl;
                //Changes arrays when we've guessed a letter correctly
                for(int i=0;i<siz;i++)
                {
                    int pos;
                    cin>>pos;
                    words[positiontoword[pos-1].first][positiontoword[pos-1].second]=guessedChar;
                }
                int index=letter_index[guessedChar];
                if(viable_letter[index]==2)
                {
                    lieDetected=true;
                }
                viable_letter[index]=1;
            }
            totalmoves+=1;
        }
        else //We guess the whole phrase
        {
            cout<<"We guess that \""<<guess.substr(0,guess.length()-1)<<"\" is the word/phrase"<<endl;
            cout<<"Is this the correct phrase? (Y) or (N)"<<endl;
            char ind;
            cin>>ind;
            if(ind=='Y')//Ends game afterwards
            {
                int phrindex=0;
                int wordindex=0;
                for(int i=0;i<guess.length();i++)
                {
                    if(guess[i]!=' ')
                    {
                        words[phrindex][wordindex]=guess[i];
                        wordindex++;
                    }
                    else
                    {
                        phrindex++;
                        wordindex=0;
                    }
                }
                totalmoves+=1;
                break;
            }
            else//Penalizes 3 questions and makes sure not to guess same phrase again
            {
                guessedPhrases.insert(guess);
                totalmoves+=3;
            }
            
        }
    }
    cout<<"Number of Questions Asked at End of Game: "<<totalmoves<<endl;
    printBoard();
}