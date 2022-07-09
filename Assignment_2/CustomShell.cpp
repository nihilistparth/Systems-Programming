// Group Number : 23
// Name : Parth Tusham 		19CS30034
// Name : Shashwat Shukla	19CS10056
// Assignment : 2

// OS Lab Sys Calls

#include<bits/stdc++.h>
#include<sstream>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>
#include<fstream>
#include <cstddef>
#include <dirent.h>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <filesystem>
#include <poll.h>

#define endl '\n'

#define DQ_MAX_SIZE 10000
#define RECENT_HISTORY 1000
#define ALPHABET_SIZE (256)
#define MAX_BUFFER_SIZE 1000
using namespace std;

pid_t shell_id ; 

#define db(...) __f(#__VA_ARGS__, __VA_ARGS__)
 
template <typename Arg1>
void __f(const char* name, Arg1&& arg1) { cout << name << " : " << arg1 << '\n'; }
template <typename Arg1, typename... Args>
void __f(const char* names, Arg1&& arg1, Args&&... args) {
    const char* comma = strchr(names + 1, ',');
    cout.write(names, comma - names) << " : " << arg1 << " | "; __f(comma + 1, args...);
}


struct termios orig_termios ;

void disableRawMode(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enableRawMode(){
    
    tcgetattr(STDIN_FILENO, &orig_termios);
    // atexit(disableRawMode);
    
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void sigintHandler(int sig_num){
    /* Reset handler to catch SIGINT next time.
       Refer http://en.cppreference.com/w/c/program/signal */
    // sigintHandler(SIGINT);
	// if(getpid() == shell_id){
	// 	return ; 
	// }
	signal(SIGINT, sigintHandler) ; 
    printf("^C\n"); 
	exit(0); 
	// return ; 
	// char* username = getenv("USER");
	// printf("@%s>>", username);
    fflush(stdout);
}


// ***********************************************************************************

void init_dq(deque<string> &dq){
	// history.txt
	// string temp = getenv("USER") ; 
	string temp = "history.txt" ; 
	ifstream infile(temp);
	// ifstream infile("history.txt");
	
	for( std::string line; getline( infile, line ); ){
		dq.push_back(line);
		// cout<<line<<endl;
	}
	
}
void save_dq(deque<string> &dq){
	// string temp = getenv("USER") ; 
	string temp = "history.txt" ;
	
	ofstream outfile(temp);
	// ofstream outfile("history.txt");
	deque<string>::iterator it = dq.begin();

  	while (it != dq.end()){
		  outfile<<*it<<endl;
		  it++;
	}
}

// ***********************************************************************************

void history_dq(deque<string>& dq){
	int i = dq.size() - 1 , cnt = 0 ; 
	cout << endl ; 
	while(i >= 0 && cnt < RECENT_HISTORY){
		cout << dq[i] << endl ; 
		i-- ; 
		cnt++ ; 
	} 
}

void help_manual(){

	cout<<"\n***WELCOME TO MY SHELL HELP***"
        "\nCopyright @ PARTH AND SHASHWAT"
        "\n-Use the shell at your own risk..."<<endl;
}

void parse(string input, char delim, vector<string>&commands) { // need to change a bit

	int command_begin = 0;
	int command_end = 0;
	for (int i = 0; input[i] != '\0'; ++i)
	{
		if (input[i] == delim )
		{
			string current;
			for (int j = command_begin; j < command_end; j++)
				current += input[j];
			commands.push_back(current);
			command_begin = i + 1;
			command_end = i + 1;
		}
		else
			++command_end;
	}
	string current;
	for (int j = command_begin; j < command_end; j++)
		current += input[j];
	commands.push_back(current);

}

// ***********************************************************************************

int parseMultiwatchCommands(string line, vector<string>& ans){
	string temp = "" ; 
    int start = 0 , s = 0 ;
    for(int i = 0 ; i < (int)line.size() ; i++){
        int c = line[i] ; 
        if(c == 34){
            start = (1 + start)%2 ;
            if(start == 0){
                ans.push_back(temp) ;
				s++ ;  
                temp = "" ; 
            } 
        }
        else{
            if(start == 1){
                temp += c ; 
            }
        }
    }
	return s ; 
}

void executeMultiWatch(string input, int in_fd, int out_fd, int pre_out, int next_in) {
	
	vector<string> tokens;

	// signal (SIGINT, SIG_DFL);
    // signal (SIGQUIT, SIG_DFL);
    // signal (SIGTSTP, SIG_DFL);
    // signal (SIGTTIN, SIG_DFL);
    // signal (SIGTTOU, SIG_DFL);

	istringstream mystream(input);
	copy(istream_iterator<string>(mystream),istream_iterator<string>(),back_inserter(tokens));
	pid_t child_pid;
    bool wait_flag = 1;
    int l = 0, r = 0, bg = 0, end = 0;
    string l_file, r_file;
	if(tokens.size() >=2){
		if(tokens[0]=="cd"){
			if(chdir(tokens[1].c_str())!=0){
				cout<<"wrong directory"<<endl;
			}
			cout<<endl;
			return;
		}
	}
	for (int i = 0; i < tokens.size(); i++){
        // cout << "token " << tokens[i] << " " ; 
        if (tokens[i] == "<"){
            l = 1; l_file = tokens[i + 1];
        }
        if (tokens[i] == ">"){
            r = 1; r_file = tokens[i + 1];
        }
        if (tokens[i] == "&"){
            bg = 1;
        }
        if (!l && !r && !bg)
            end++;
    }
    cout << endl;
    const char **argv = new const char *[end + 1];
    for (int i = 0; i < end; i++){
        argv[i] = tokens[i].c_str();
    }
    argv[end] = NULL;
    wait_flag = !bg;
    
	// if ((child_pid = fork()) == 0){
		signal(SIGTSTP,SIG_DFL);
		signal(SIGINT,SIG_DFL);
        if (r){
			out_fd = open(r_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
		}
        if (out_fd != 1){
            dup2(out_fd, 1);
			close(out_fd) ; 
		}
        if (l){
			in_fd = open(l_file.c_str(), O_RDONLY);
		}
        if (in_fd != 0){
			dup2(in_fd, 0);
			close(in_fd) ; 
		}

		if( execvp( tokens[0].c_str(), (char **)argv )  < 0) {
			cout<<"Could not execute command.."<< endl;
		}
		
        exit(0);
    // }

	// else{
		int status;
		if(bg ==false){
			do
			{
				int id = waitpid(child_pid,&status,WUNTRACED);  // shai chla ya nhi
			} while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
			if(WIFSIGNALED(status)){
				cout<<endl;
			}
			if(WIFSTOPPED(status)){
				kill(child_pid,SIGCONT);
				cout<<endl;
			}
		}
		
		if (wait_flag){  //Wait for the child process to get completed
			// wait(NULL);
			if (in_fd != 0)
				close(in_fd);
			if (out_fd != 1)
				close(out_fd);
		}
		else
			cout << "[ Process ] Background" << endl;
	// }
    return;
}

int multiWatch(string input){

	int terminal_fd = STDIN_FILENO ; 

	pid_t extraChild, extraChild_grp = 0 ; 
	extraChild = fork() ; 
	
	if(extraChild == 0){

		signal(SIGINT, SIG_DFL) ; 
		// signal(SIGTSTP, SIG_DFL) ; 
		extraChild = getpid() ; 
		if(!extraChild_grp){
			extraChild_grp = extraChild ; 
		}
		setpgid(extraChild, extraChild_grp) ; 
		// tcsetpgrp(terminal_fd, extraChild_grp) ; 
		
		vector<string> comm ; 

		if(parseMultiwatchCommands(input, comm) <= 0){
			exit(-1) ; 
		}	

		int numCommands = comm.size() ; 

		vector<int> writefds, readfds ; 

		int pipes[numCommands][2];
        for(int i = 0; i < numCommands; i++) {
            if(pipe(pipes[i]) < 0) {
                cerr<<"ERROR:: pipe() failed."<<endl;
                exit(-1);
            }
            int fd = pipes[i][1];
            writefds.push_back(fd);

            int rfd = pipes[i][0];
            readfds.push_back(rfd);
        }

		// db(numCommands) ; 

		vector<pid_t> processes;
        for(int i = 0; i < numCommands; i++) {
            pid_t pid;
            pid = fork();
			
			if(pid == 0) {
				dup2(writefds[i], STDOUT_FILENO);
				close(writefds[i]);
				
				vector<string> tpComm ; 
				parse(comm[i], '|', tpComm) ; 
				
				int number_of_pipes = tpComm.size() - 1;
				int tppipes[number_of_pipes][2];
				
				for (int j = 0; j < number_of_pipes ; j++) {
					pipe(tppipes[j]);
				}
				
				for (int j = 0; j < tpComm.size(); j++) {
					int in_fd = 0 , out_fd = 1, pre_out = -1, next_in = -1;
					if (i > 0) {
						in_fd = tppipes[j - 1][0];
						pre_out = tppipes[j - 1][1];
					}
					else if (j < number_of_pipes) {
						out_fd = tppipes[j][1];
						next_in = tppipes[j][0];
					}
					// cout << "Debug Command " <<tpComm[j] << endl ; 
					executeMultiWatch(tpComm[j], in_fd, out_fd, pre_out, next_in);
				}
				// executeMultiWatch(comm[i], STDIN_FILENO, STDOUT_FILENO, -1, -1);	
				exit(1);
			}

			else{
				processes.push_back(pid);
			}
        } 

		int nfds, openfds;
        struct pollfd* pollfdset;
        openfds = nfds = numCommands;

        pollfdset = (struct pollfd*)calloc(nfds, sizeof(struct pollfd));
        if(pollfdset == NULL) {
            cerr<<"ERROR:: calloc() failed."<<endl;
        }

        for(int i = 0; i < nfds; i++) {
            pollfdset[i].fd = pipes[i][0];
            pollfdset[i].events = POLLIN;
        }

		// db(openfds, nfds); 

		int c = 0 ;
        while(openfds > 0) {

			// signal(SIGINT, sigintHandler) ; 
			int ready = poll(pollfdset, nfds, -1);
			// signal(SIGINT, sigintHandler) ;  

            if(ready == -1) {
                cerr<<"ERROR:: bad poll()."<<endl;
                exit(-1);
            }
			// db(c, c) ; 
            for(int j = 0; j < nfds; j++) {

			    char buffer[MAX_BUFFER_SIZE];
                
				if (pollfdset[j].revents != 0) {
                    // db(pollfdset[j].revents) ; 
					// printf("  fd=%d; events: %s%s%s\n", pollfdset[j].fd, (pollfdset[j].revents & POLLIN)  ? "POLLIN "  : "",(pollfdset[j].revents & POLLHUP) ? "POLLHUP " : "",(pollfdset[j].revents & POLLERR) ? "POLLERR " : "");
                    if (pollfdset[j].revents & POLLIN) {
						
						memset(buffer, 0, MAX_BUFFER_SIZE);
                        ssize_t s = read(pollfdset[j].fd, buffer, sizeof(buffer));
						// db(s) ; 
                        if (s == -1) {
                            cerr<<"ERROR:: read() failed."<<endl;
                        }
						if(s > 1){
							cout<<endl;
							cout<<"\""<<comm[j]<<"\", current_time:"<<(unsigned long)time(NULL)<<endl;
							cout<<"<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-<-"<<endl;
							cout<<buffer;
							cout<<"->->->->->->->->->->->->->->->->->->->"<<endl<<endl;
						}
                        
                    } 
                    else {                /* POLLERR | POLLHUP */
                        // printf("    closing fd %d\n", pollfdset[j].fd);
                        if (close(pollfdset[j].fd) == -1) {
                            cerr<<"ERROR:: close() failed."<<endl;
                        }
						openfds--;
                    }
                }
            }
        }
        
        return 0;
	}
	
	else{

		if(!extraChild_grp) extraChild_grp = extraChild; 
        setpgid(extraChild, extraChild_grp);
		
        // tcsetpgrp(terminal_fd, extraChild_grp);
		signal(SIGINT, sigintHandler) ;
		// signal(SIGINT, SIG_IGN) ;
        // tcsetpgrp(extraChild, extraChild_grp);
        
		int status;
        pid_t x = waitpid(extraChild, &status, WUNTRACED);
        
		// cout<<"Wait Status : "<<status<<endl;
        // cout<<"Waiting stopped.."<<endl;
        
        // tcsetpgrp(terminal_fd, shell_pgid);
        // struct termios job_tmodes;
        // tcgetattr(terminal_fd, &job_tmodes);

        // tcsetattr(terminal_fd, TCSADRAIN, &shell_tmodes);
        if(WIFEXITED(status)) {
            return WEXITSTATUS(status);
        }
        return 0;
	}
}

// ***********************************************************************************

int execute_custom_command(string input, deque<string>& dq) {
	if (input == "exit") {
		printf("\n") ; 
		return 2;
	}
	
	if(input == "help"){
		help_manual();
		return 1; 
	}

	if(input == "history"){
		history_dq(dq) ;
		return 1 ;  
	}

	// implement cd;

	return 0;
}

void execute(string input, int in_fd, int out_fd, int pre_out, int next_in) {
	vector<string> tokens;

	// signal (SIGINT, SIG_DFL);
    // signal (SIGQUIT, SIG_DFL);
    // signal (SIGTSTP, SIG_DFL);
    // signal (SIGTTIN, SIG_DFL);
    // signal (SIGTTOU, SIG_DFL);

	istringstream mystream(input);
	copy(istream_iterator<string>(mystream),istream_iterator<string>(),back_inserter(tokens));
	pid_t child_pid;
    bool wait_flag = 1;
    int l = 0, r = 0, bg = 0, end = 0;
    string l_file, r_file;
	if(tokens.size() >=2){
		if(tokens[0]=="cd"){
			if(chdir(tokens[1].c_str())!=0){
				cout<<"wrong directory"<<endl;
			}
			cout<<endl;
			return;
		}
		else if(tokens[0] == "multiwatch"){
			int c = multiWatch(input) ; 
			return ; 
		}
	}
	for (int i = 0; i < tokens.size(); i++){
        // cout << "token " << tokens[i] << " " ; 
        if (tokens[i] == "<"){
            l = 1; l_file = tokens[i + 1];
        }
        if (tokens[i] == ">"){
            r = 1; r_file = tokens[i + 1];
        }
        if (tokens[i] == "&"){
            bg = 1;
        }
        if (!l && !r && !bg)
            end++;
    }
    cout << endl;
    const char **argv = new const char *[end + 1];
    for (int i = 0; i < end; i++){
        argv[i] = tokens[i].c_str();
    }
    argv[end] = NULL;
    wait_flag = !bg;
    
	if ((child_pid = fork()) == 0){
		signal(SIGTSTP,SIG_DFL);
		signal(SIGINT,SIG_DFL);
        if (r){
			out_fd = open(r_file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0644);
		}
        if (out_fd != 1){
            dup2(out_fd, 1);
			close(out_fd) ; 
		}
        if (l){
			in_fd = open(l_file.c_str(), O_RDONLY);
		}
        if (in_fd != 0){
			dup2(in_fd, 0);
			close(in_fd) ; 
		}

		if( execvp( tokens[0].c_str(), (char **)argv )  < 0) {
			cout<<"Could not execute command.."<< endl;
		}
		
        exit(0);
    }

	else{
		int status;
		if(bg ==false){
			do
			{
				int id = waitpid(child_pid,&status,WUNTRACED);  // shai chla ya nhi
			} while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));
			if(WIFSIGNALED(status)){
				cout<<endl;
			}
			if(WIFSTOPPED(status)){
				kill(child_pid,SIGCONT);
				cout<<endl;
			}
		}
		
		if (wait_flag){  //Wait for the child process to get completed
			// wait(NULL);
			if (in_fd != 0)
				close(in_fd);
			if (out_fd != 1)
				close(out_fd);
		}
		else
			cout << "[ Process ] Background" << endl;
	}
    return;
}

// ***********************************************************************************



// ***********************************************************************************



// ***********************************************************************************

int check_substring_match(string input, string h){
	int n = h.size() , m = input.size() ; 

	int LCS[n+1][m+1] , ans = 0 ; 
	for(int i = 0 ; i <= n ; i++){
		for(int j = 0 ; j <= m ; j++){
			if(i == 0 || j == 0){
				LCS[i][j] = 0 ; 
			}
			else if(h[i-1] == input[j-1]){
				LCS[i][j] = LCS[i-1][j-1] + 1 ; 
				ans = max(LCS[i][j], ans) ; 
			}
			else{
				LCS[i][j] = 0 ; 
			}
		}
	}
	return ans ; 
}

void search_from_history(deque<string>& dq){
	
	disableRawMode() ; 
	cout << "Enter the search term :" ; 
	string inp ;  
	getline(cin, inp) ; 
	int n = dq.size() - 1 ;
	vector<string> full_match ;
	vector<pair<int, string>> partial_match ;  
	for(int i = n ; i >= 0 ; i--){
		int a = check_substring_match(inp, dq[i]) ; 
		if(a == inp.size() && a >= 3){
			full_match.push_back(dq[i]) ; 
		}
		else if(a < inp.size() && a >= 3){
			partial_match.push_back({-1*a, dq[i]}) ; 
		}
	}

	if(!full_match.empty()){
		for(auto x : full_match){
			cout << x << endl ; 
		}
		return ; 
	}

	if(!partial_match.empty()){
		sort(partial_match.begin(), partial_match.end()) ; 
		int prev = -1*partial_match[0].first , i = 0 ;
		for(int i = 0 ; i < n ; i++){
			if(prev > -1*partial_match[i].first){
				break ; 
			}
			else{
				cout << partial_match[i].second << endl ; 
			}
		} 
		return ;
	}

	cout << "No Match for search term in history" << endl ; 

}

//*******************************************************************************

int char_to_idx(char c){
    return (int)c ; 
}

struct TrieNode{
    struct TrieNode *children[ALPHABET_SIZE];
    bool isWordEnd;
};

struct TrieNode *getNode(void){
    struct TrieNode *pNode = new TrieNode;
    pNode->isWordEnd = false;
 
    for (int i = 0; i < ALPHABET_SIZE; i++)
        pNode->children[i] = NULL;
 
    return pNode;
}


void insert(struct TrieNode *root,  const string key){
    struct TrieNode *pCrawl = root;
 
    for (int level = 0; level < key.length(); level++)
    {
        int index = char_to_idx(key[level]);
        if (!pCrawl->children[index])
            pCrawl->children[index] = getNode();
 
        pCrawl = pCrawl->children[index];
    }
 
    pCrawl->isWordEnd = true;
}

bool search(struct TrieNode *root, const string key){
    int length = key.length();
    struct TrieNode *pCrawl = root;
    for (int level = 0; level < length; level++)
    {
        int index = char_to_idx(key[level]);
 
        if (!pCrawl->children[index])
            return false;
 
        pCrawl = pCrawl->children[index];
    }
 
    return (pCrawl != NULL && pCrawl->isWordEnd);
}
 
bool isLastNode(struct TrieNode* root){
    for (int i = 0; i < ALPHABET_SIZE; i++)
        if (root->children[i])
            return 0;
    return 1;
}
 
void suggestionsRec(struct TrieNode* root, string currPrefix, vector<string>&ans){
    if (root->isWordEnd){
        ans.push_back(currPrefix);
    }
 
    if (isLastNode(root))
        return;
 
    for (int i = 0; i < ALPHABET_SIZE; i++){

        if (root->children[i]){
            currPrefix.push_back(i) ; 
            suggestionsRec(root->children[i], currPrefix, ans);
            currPrefix.pop_back();
        }
    }
}
 
int getAutoSuggestions(TrieNode* root, const string query, vector<string>&ans){
    struct TrieNode* pCrawl = root;
 
    int level;
    int n = query.length();
    for (level = 0; level < n; level++){
        int index = char_to_idx(query[level]);
 
        if (!pCrawl->children[index])
            return 0;
 
        pCrawl = pCrawl->children[index];
    }
 
    bool isWord = (pCrawl->isWordEnd == true);
    bool isLast = isLastNode(pCrawl);
 	signal(SIGTSTP, SIG_IGN);
    if (isWord && isLast){
        cout << query << endl;
        return -1;
    }
    
    if (!isLast){
        string prefix = query;
        suggestionsRec(pCrawl, prefix, ans);
        return 1;
    }

    return -1 ; 
}

void loadTrie(struct TrieNode* root){

    DIR *dir ; 
    struct dirent *diread ; 
    
    if( (dir = opendir("./")) != nullptr ){
        while ((diread = readdir(dir)) != nullptr) {
            const string tmp = diread->d_name ;  
            insert(root, tmp) ; 
        }
        closedir(dir) ; 
    }

}

void tabAutocomplete(string temp, string& inp){
	struct TrieNode* root = getNode();
	loadTrie(root) ; 
	string str ; 
	if(temp[0] == '.' && temp[1] == '/'){
		str = temp.substr(2) ; 
	}
	else{
		str = temp ; 
	}

	if(str.size() < 2){
		return ; 
	}

	vector<string> result ; 
	int v = getAutoSuggestions(root, str, result) ;

	if(v == -1)	return ; 

	if(result.size() == 1){
			cout << result[0].substr(str.size()) ; 
			inp += result[0].substr(str.size());
	}
	else{
		cout << endl ; 
		for(int i = 0 ; i <(int)result.size() ; i++){
			printf("%d. %s\n", i+1, result[i].c_str()) ; 
		} 
		disableRawMode() ; 
		int x  ; cin >> x ;
		if(x > result.size() || x <= 0){
			return ; 
		}
		cout << inp << result[x-1].substr(str.size()) << endl ; 
		inp += result[x-1].substr(str.size()) ; 
	}	
	fflush(stdout) ; 
}

// ***********************************************************************************

int main() {

	shell_id = getpid() ; 

	string input;
	vector<string> custom_commands = {"exit","help", "history"};
	// signal(SIGTSTP, SIG_IGN); // implement ctrl+z
	// signal(SIGINT, SIG_IGN); // implement ctrl+C

	signal (SIGINT, SIG_IGN);
    signal (SIGQUIT, SIG_IGN);
    signal (SIGTSTP, SIG_IGN);
    signal (SIGTTIN, SIG_IGN);
    signal (SIGTTOU, SIG_IGN);


	deque<string> dq;
	init_dq(dq); // read lines from history.txt
	
	while (1) {
		start:
		char dir[256];
		getcwd(dir,256);
		char* username = getenv("USER");
		// signal(SIGTSTP, SIG_IGN); // implement ctrl+z
		// signal(SIGINT, SIG_IGN); // implement ctrl+C
		
		printf("@%s:%s>>", username,dir);
		fflush(stdout) ; 
		// getline(cin, input);
		enableRawMode() ;

		input = "" ;  
		string temp = "" ; 
		char c ; 
		int c_flag = 0 ; 
			
		while (read(STDIN_FILENO, &c, 1) == 1){
			if(iscntrl(c)){
				int val = c ; 
				if(val == 127){
					if(!input.empty()){
						input.pop_back();
					}
					if(!temp.empty()){
						temp.pop_back() ; 
					}
					printf("\b") ;
					printf(" ") ;
					printf("\b") ; 
				}
				else if(val == 3){
					// ctrl C will work
					printf("^C\n") ; 
					c_flag = 1 ; 
					break ; 
				}
				else if(val == 9){
					// tab will work
					tabAutocomplete(temp, input) ; 
					//execute after tab
					// c_flag = 3 ; 
				}
				else if(val == 18){
					// control R will work
					search_from_history(dq) ; 
					c_flag = 2 ; 
					break ; 
				}
				else if(val == 26){
					// ctrl Z will work
				}
				else if(val == 10){
					// Enter key will work
					break ; 
				}
			}
			else{
				if(c == ' '){
					temp = "" ;  
				}
				else{
					temp += c ;  
				}
				input += c ;
				printf("%c", c) ; 
			}
			fflush(stdout) ;
		}

		disableRawMode() ;

		if(c_flag >= 1){
			continue ; 
		}

		// signal(SIGINT, sigintHandler);
		// changes end
		
		dq.push_back(input);
		if(dq.size() > DQ_MAX_SIZE){
			dq.pop_front();
		}
		
		int custom_command_ret_value = 0;
		for (auto cmd : custom_commands) {
			if (input == cmd) {
				custom_command_ret_value = execute_custom_command(input, dq);
			}
		}
		if (custom_command_ret_value > 0) {
			save_dq(dq);// overwrite history.txt
			if(custom_command_ret_value > 1)	break;
			else 	continue ; 
		}
		string tempCommand = "" ; 
		for(int i = 0 ; i < min(10, (int)input.size()) ; i++){
			tempCommand += input[i] ; 
		}

		if(tempCommand == "multiWatch"){
			cout << endl ; 
			multiWatch(input); 
			continue;
		}

		vector<string> commands;
		parse(input, '|' , commands);

		//making streams for pipi operation
		int number_of_pipes = commands.size() - 1;
		int pipes[number_of_pipes][2];
		for (int i = 0; i < number_of_pipes ; i ++) {
			pipe(pipes[i]);
		}
		for (int i = 0; i < commands.size(); i++) {
			int in_fd = 0 , out_fd = 1, pre_out = -1, next_in = -1;
			if (i > 0) {
				in_fd = pipes[i - 1][0];
				pre_out = pipes[i - 1][1];
			}
			else if (i < number_of_pipes) {
				out_fd = pipes[i][1];
				next_in = pipes[i][0];
			}
			execute(commands[i], in_fd, out_fd, pre_out, next_in);
		}
	}
}

/*
multiWatch ["ls", "ps", "date"]
*/