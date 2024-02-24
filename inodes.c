#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "inodes.h"


void loadINodeList(const char *path){			//Requirement 2: Loads the Inode List and Checks for the Incorrect Cases
        FILE *inode_file = fopen(path, "rb");		//Open Inode List File in Read Binary Mode
        if (!inode_file){				//Check if Inode List File Exists if not then Terminate
                printf("Inode File Doesn't Exist\n");	
                exit(1);
        }

        inodeCount = 0;
        while (fread(&inodeList[inodeCount], sizeof(Inode), 1, inode_file) == 1){		//Reads the Inode List File and Checks the Types and Inode Numbers
                if (inodeList[inodeCount].type != 'd' && inodeList[inodeCount].type != 'f'){
                        printf("Incorrect Type\n");
                }
                if (inodeList[inodeCount].node >= 1024){
                        printf("Incorrect Inode Number\n");
                }
                inodeCount++;
        }

        if (inodeList[0].node != 0 || inodeList[0].type != 'd'){   //Verifies that the Initial Inode is a Directory and if not then Terminate
        	printf("Initial Inode is not a Directory\n");
        	fclose(inode_file);
                exit(1);
        }
        fclose(inode_file);	//Close the Inode File
} 	


void saveINodeList(const char *path){			//Requirement 3,4: Saves the Inode List and Updates any Changes made
	FILE *inode_file = fopen("inodes_list", "wb");  //Open the Inode List File in Write Binary Mode
        if (!inode_file){				//Check for Inode File Existence if not then Terminate
        	printf("Inodes File Doesn't Exist\n");
        	exit(1);
        }

        size_t nodesWritten = fwrite(inodeList, sizeof(Inode), inodeCount, inode_file);   //Variable to Write Nodes for Updated Inode List
        if (nodesWritten != inodeCount){						  //Check for any Error with Writing if there is then Terminate
        	printf("Error Writing INode List to File\n");
        	fclose(inode_file);
        	exit(1);
        }
        fclose(inode_file);	//Close the Inode File                                                                                                                                                                                                                                             
}


void changeDirectory(const char *name){			//Requirement 3: Function that Changes ti Specified Directory
        if (name == NULL){				//Checks if there is a Invalid Name Provided
                printf("Invalid Directory Name\n");
                return;
        }

        for (size_t i = 0; i < inodeCount; i++){							    //Iterate Through the Inode List
                if (strcmp(inodeList[i].name, name) == 0 && inodeList[i].parentInode == currentInode){	    //Check if Inode Matches Provided Name and if Parent Inode Matches Current Inode
                        if (inodeList[i].type == 'd'){							    //Checks if Inode Type is a Directory, if it is then it Changes to that Directory and Updates Current Inode
                                printf("Changing to Directory: %s (Inode %u)\n", name, inodeList[i].node);
                                currentInode = inodeList[i].node;
                                return;
                        } else {									    //Else Report that the Parameter is not a Directory
                                printf("%s is not a Directory\n", name);
                                return;
                        }
                }
        }
        printf("Directory not Found\n");								    //If Nothing Happens in the For Loop then Report Directory Not Found
}


void listContents(){							//Requirement 3: Function that Lists Contents of the Current Directory
        printf("Contents of Current Directory %u:\n", currentInode);	
        for (size_t i = 0; i < inodeCount; i++){												 //Iterate Through the Inode List
                if (inodeList[i].parentInode == currentInode){											 //If Parent Node Matches Current Node, then Print Inode Details
                        printf("Inode %u: %s (%s)\n", inodeList[i].node, inodeList[i].name, (inodeList[i].type == 'd') ? "Directory" : "File");
                }
        }
}


void createDirectory(const char *name){			//Requirement 3: Function that Creates a Specified Directory
        if (name == NULL){				//Checks if there is a Invalid Name Provided
                printf("Invalid Directory Name\n");
                return;
        }	

        for (size_t i = 0; i < inodeCount; i++){							  //Iterate Through Inode List
                if (inodeList[i].parentInode == currentInode && strcmp(inodeList[i].name, name) == 0){	  //Check if the Directory Already Exists
                        printf("Directory Already Exists\n");
                        return;
                }
        }
        if (inodeCount >= 1024){								//Check if There are Inodes Avaliable
                printf("No Inodes Avaliable");
                return;
        }
												//Update the Inode List
        inodeList[inodeCount].node = inodeCount;						//Give the Node a Inode Number
        inodeList[inodeCount].parentInode = currentInode;					//Set the Parent Inode
        inodeList[inodeCount].type = 'd';							//Set the Inode Type Directory
        strncpy(inodeList[inodeCount].name, name, sizeof(inodeList[inodeCount].name));		//Copy the Name of the Directory to the Name Part of the Inode
        inodeCount++;										//Increment Inode Count by 1
        printf("Successfully Created Directory %s\n", name);				        //Report Successfully Created Directory
}


void createFile(const char *name){								//Requirement 3: Function that Creates a Specified File
        if (name == NULL){									//Check if there is a Invalid Name Provided
                printf("Invalid File Name\n");
                return;
        }

        for (size_t i = 0; i < inodeCount; i++){							  //Iterate Through the Inode List
                if (inodeList[i].parentInode == currentInode && strcmp(inodeList[i].name, name) == 0){	  //Check if the File Already Exists
                        printf("File Already Exists\n");
                        return;
                }
        }
        if (inodeCount >= 1024){								//Check for Avaliable Inodes
                printf("No Inodes Avaliable");
                return;
        }
												//Similar to createDirectory, Update Inode List
        inodeList[inodeCount].node = inodeCount;						//Give the Node a Inode Number
        inodeList[inodeCount].parentInode = currentInode;					//Set its Parent Inode
        inodeList[inodeCount].type = 'f';							//Set the Inode Type to File
        strncpy(inodeList[inodeCount].name, name, sizeof(inodeList[inodeCount].name));		//Copy the File Name to the Name Part of the Inode
        inodeCount++;										//Increment Inode Count by 1
        printf("Successfully Created File %s\n", name); 					//Report Sucessful Creaton of File
}


int main(int argc, char *argv[]){			 //main
        if (argc != 2){					 //Check if the Number of Commands Are Incorrect
                printf("Invalid Amount of Commands\n");
                exit(1);
        }
        const char *path = argv[1];			//Set the Path for the Directory to go into to be argv[1]
        const char *list = "inodes_list";		//Variable to Hold the inodes_list File Name
        if (chdir(path) == -1){                         //Requirement 1: Verifies that the Specified Directory Provided Exists if chdir Fails and Returns -1. If chdir Works then we Change into Provided Directory
                printf("Directory Doesn't Exist");
                return 1;
        } else {
                printf("Directory %s Exists\n", path);
        }
        loadINodeList(list);				//Loads the Inode List in that Directory

        while(1){					//While Loop While User is Running the Program
                printf("Current Directory: %u\n", currentInode);
                printf("Enter 'cd directory_name' to Change Directory, 'ls' to List Contents of the Directory, 'mkdir directory_name' to Create a Directory, 'touch directory_name' to Create a File, and 'exit' to Save and Exit the Program\n");
                printf(">> ");

                char input [32];				   //Array to Store the User Input
                if (fgets(input, sizeof(input), stdin) == NULL){   //Check if the Input is Invalid 
                        printf("Invalid Input\n");
                        exit(1);
                }

                size_t length = strlen(input);			   //Length Variable to Keep Track of Length of User Input
                if (length > 0 && input[length-1] =='\n'){	   //Checks for Valid Length and for Newline, and then Replaces the New Line with '\0'
                        input[length-1] = '\0';
                }

                char command[32];				   //Array to Keep Track of the Specified Command such as cd, ls, mkdir, etc
                char *name = NULL;				   //Pointer Variable to Keep Track of any File or Directory Name Following the Command
                size_t i = 0;					   
                while(input[i] != ' ' && input[i] != '\0'){	   //Loops Through Each Character in the User Input Until it Reaches a Space or End of String
                        command[i] = input[i];			   //Stores the Characters from Input to Command
                        i++;					   //Increment i by 1
                }
                command[i] = '\0';				   //Adds a '\0' Terminating Character to the End of the Command Array
                while (input[i] == ' '){			   //Check if the Character in Input is ' ' and Increment Over it
                        i++;
                }

                if (strcmp(command, "cd") == 0 || strcmp(command, "mkdir") == 0 || strcmp(command, "touch") == 0){   //Check if it is any of the Commands which are Followed with a name
                        if (input[i] != '\0'){									     //If there are Characters after the Command Part of the Input, it sets the Name of the File/Directory to the rest of the characters
                                name = &input[i];
                                printf("File or Directory Name: %s\n", name);
                        }
                }
                if (strcmp(command, "cd") == 0) {		//Function Call if cd is Entered
                        changeDirectory(name);		
                } else if (strcmp(command, "ls") == 0) {	//Function Call if ls is Entered
                        listContents();
                } else if (strcmp(command, "mkdir") == 0){	//Function Call if mkdir is Entered
                        createDirectory(name);
                } else if (strcmp(command, "touch") == 0){	//Function Call if touch is Entered
                        createFile(name);
                } else if (strcmp(command, "exit") == 0){	//Function Call if exit is Entered
                        saveINodeList(list);
                        break;
                } else {
                        printf("Command Not Recognized\n");	//Print Statement if there is a Unrecognized Command
                }
        }
        return 0;
}

