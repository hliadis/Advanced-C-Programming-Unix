#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>


off_t search_file(int fd_base ,char **ID,char **fileName);
int blocksOfData(int fd_base, int fd , char* blockOfData,int fileLength);

char print_menu(){
	
	char action_value;
	
	do{
		printf("********************\n");
		printf("MAIN MENU:\n");
		printf("i(mport)<name>\n");
		printf("f(ind)<name>\n");
		printf("e(xport)<src><dest>\n");
		printf("d(elete)<name>\n");
		printf("q(uit)\n");
		printf("********************\n");
		printf("Enter your action: ");
		scanf(" %c",&action_value);
		
		if (!((action_value == 'i') || (action_value == 'e') || (action_value == 'd') || (action_value == 'q')|| (action_value == 'f'))){
			printf("ERROR: Not a valid character.Please insert one of the characters i,f,e,d,q.\n");
		}
	}while(!((action_value == 'i') || (action_value == 'e') || (action_value == 'd') || (action_value == 'q')|| (action_value == 'f')));
	
	return( action_value);
	
}

void import(int fd_base){
	
	char *filePath,*fileName,*IDimport,format2[5], *IDfinal;
	char *blockOfData,*existingFileName = NULL,*ID = NULL; 
	int fdImportedFile;
	int fileLength;
	struct stat  fileInfo;
	off_t sizeOfFile;
	ssize_t read_value,write_value;
	
	
	
	/*max characters linux allows for a filepath 4096*/
	filePath = (char*)malloc(sizeof(char) * 4097);
	if (filePath == NULL){
		printf("ERROR: Unable to allocate memory\n");
		exit(-1);
	}
		
	printf("Enter the relative or the absolute file path of the file :\n");
	scanf(" %4096s",filePath);
	
	/*Free the useless memmory that is allocated.*/
	filePath =(char*)realloc(filePath,strlen(filePath)+1);
	if (filePath == NULL){
		printf("ERROR: Unable to allocate memory\n");
		exit(-1);
	}
	
	/*Finding the file name from the path*/
	fileName = strrchr(filePath,'/');
	if (fileName == NULL){
		fileName = filePath;
	}
	else{
		fileName = fileName + 1;
	}
	
	//Setting offset exactly after the magic start number
	if(lseek(fd_base,5,SEEK_SET) == -1){
		printf("%s\n",strerror(errno));
		free(filePath);
		exit(-1);
	}
	
	//Checking if in the database exists a file with the
	//same name.
	do{
		fileLength = search_file(fd_base,&ID,&existingFileName);
		if (fileLength == -1){
			free(filePath);
			exit(-1);
		}
		if (fileLength == -2){
			free(ID);
			break;
		}
		if(strcmp(existingFileName,fileName) == 0){
			printf("ERROR: File already exists in database!\n");
			free(ID);
			free(filePath);
			return;
		}
	}while(1);
	
	/*Setting offset to the end of the file before the
	 magicEndNumber*/
	if (lseek(fd_base,-4,SEEK_END) == -1){
		printf("%s\n",strerror(errno));
		free(filePath);
		exit(-1);
	}
	
	/*Opening the file given by the user and creating the 
	 ID for the file(0000.fileName).First 4 bytes indicate the
	 length of the following name including the dot.*/ 
	fdImportedFile = open(filePath,O_RDWR);
	if (fdImportedFile == -1){
		printf("%s\n",strerror(errno));
		free(filePath);
		return;
	}
	
	IDimport = (char*)malloc((3+strlen(fileName))*sizeof(char));
	IDfinal = (char*)malloc(7+strlen(fileName)*sizeof(char));
	if (IDimport == NULL){
		printf("ERROR: Unable to allocate memory\n");
		free(filePath);
		exit(-1);
	}

	strcpy(IDimport,".");
	IDimport = strcat(IDimport,fileName);
	sprintf(format2,"%04ld",strlen(IDimport));
	strcpy(IDfinal,format2);
	IDfinal = strcat(IDfinal,IDimport);
	
	write_value = write(fd_base, IDfinal, strlen(IDfinal));
	free(IDimport);
	free(IDfinal);
	if (write_value == -1){
		printf("%s\n",strerror(errno));
		free(filePath);
		exit(-1);		
	}
	
	//Import the length of the file
	if (fstat(fdImportedFile,&fileInfo) == -1){
		printf("%s\n",strerror(errno));
		free(filePath);
		exit(-1);	
	}
	
	sizeOfFile = fileInfo.st_size;
	write_value = write(fd_base,&sizeOfFile,sizeof(off_t));
	if (write_value == -1){
		printf("%s\n",strerror(errno));
		free(filePath);
		exit(-1);		
	}
	//Transfer data with blocks of 512 bytes.
	blockOfData = (char*)malloc(512*sizeof(char));
	if (blockOfData == NULL){
		printf("ERROR: Unable to allocate memory\n");
		free(filePath);
		exit(-1);
	}
	do{
		
		read_value = read(fdImportedFile,blockOfData,512);
		if (read_value == -1){
			printf("%s\n",strerror(errno));
			free(blockOfData);
			free(filePath);
			exit(-1);		
		}
		
		write_value = write(fd_base,blockOfData,read_value);
		if (write_value == -1){
			printf("%s\n",strerror(errno));
			free(blockOfData);
			free(filePath);
			exit(-1);		
		}
		if (read_value < 512){
			break;
		}
	}while(1);
	
	//Rewrite the end magic number.Because it has been overwritten by the file.
	write_value = write(fd_base,"\xec\xaf\xfa\xce",4);
	if (write_value == -1){
		printf("%s\n",strerror(errno));
		free(blockOfData);
		free(filePath);
		exit(-1);		
	}
	if (write_value < 4){
		printf("ERROR: Unable to write important information.\n");
		free(blockOfData);
		free(filePath);
		exit(-1);
	}
	if(fsync(fd_base)==-1){
		printf("%s\n",strerror(errno));
		free(blockOfData);
		free(filePath);
		exit(-1);
	}
	
	free(filePath);
	free(blockOfData);
	close(fdImportedFile);
	printf("********************\n");
	printf("The file imported successfully!\n");
}

void find(int fd_base){
	
	char *givenName , *fileName = NULL, *ID = NULL;
	off_t fileLength;
	int i = 1;
	
	//Set the offset at the point after the magic_startNum
	if (lseek(fd_base,5,SEEK_SET) == -1){
		printf("%s\n",strerror(errno));
		exit(-1);
	}
	
	//Max length of file name in a wide variety of file systems is 255.
	//The part of the code where the user write the file he/she wants
	//to search
	printf("Type a file name or a part of a file name :\n");
	givenName = (char*)malloc(256*sizeof(char));
	if (givenName == NULL){
		printf("ERROR: Unable to allocate memory\n");
		exit(-1);
	}
	scanf(" %255s",givenName);
	givenName = (char*)realloc(givenName,strlen(givenName)+1);
	if (givenName == NULL){
		printf("ERROR: Unable to allocate memory\n");
		exit(-1);
	}
	
	//The main part of find function.
	if (strcmp(givenName,"*") == 0){
		printf("All the files in the database are: \n");
		do{
			/*search_file returns the length of the imported file .
			 *It also returns the name of the file*/
			//It sets the file offset right before the next ID.
			fileLength = search_file(fd_base,&ID,&fileName);
			
			if (fileLength == -1){
				free(givenName);
				exit(-1);
			}
			if (fileLength == -2){
				free(ID);
				break;
			}
			
			printf("%d)%s\n",i,fileName);
			i++;
			free(ID);
		}while(1);
	}
	else{
		printf("The relative files names in the database are: \n");
		do{
			
			fileLength = search_file(fd_base,&ID,&fileName);
			if (fileLength == -1){
				free(givenName);
				exit(-1);
			}
			if (fileLength == -2){
				free(ID);
				break;
			}
			if (strstr(fileName,givenName) != NULL){
				printf("%d)%s\n",i,fileName);
				i++;
			}
			free(ID);
		}while(1);
	}	
	free(givenName);
	return;
}

void export(int fd_base){
	
	char *exportedFile, *ID = NULL, *existingFileName = NULL, *inputFile, *blockOfData;
	int fd, i;
	off_t fileLength, oddBytes, numOfBlocks;
	
	//Set the offset at the point after the magic_startNum
	if (lseek(fd_base,5,SEEK_SET) == -1){
		printf("%s\n",strerror(errno));
		exit(-1);
	}
	//
	printf("Enter the filename you want to export:\n");
	exportedFile = (char *)malloc(256*sizeof(char));
	if (exportedFile == NULL) {
		printf("ERROR: Unable to allocate memory\n");
		exit(-1);
	}
	
	scanf(" %255s", exportedFile);
	exportedFile = (char *)realloc(exportedFile, strlen(exportedFile)+1);
	if (exportedFile == NULL) {
		printf("ERROR: Unable to allocate memory\n");
		exit(-1);
	}
	//Checking if the file already exists in the database.
	do{
		fileLength = search_file(fd_base,&ID,&existingFileName);
		if (fileLength == -1){
			free(exportedFile);
			exit(-1);
		}
		if (fileLength == -2){
			printf("File %s is not in the database!\n", exportedFile);
			free(ID);
			free(exportedFile);
			return;
		}
		if (strcmp(existingFileName,exportedFile) == 0){
			printf("File found in the database!\n");
			free(ID);
			break;
		}
	}while(1);
	
	do{
		printf("Enter the absolut or the relative path of the destination file:\n");
	
		inputFile = (char *)malloc(4097*sizeof(char));
		if (inputFile == NULL) {
			printf("ERROR: Unable to allocate memory\n");
			free(exportedFile);
			exit(-1);
		}
		
		scanf(" %4096s", inputFile);
		
		inputFile = (char *)realloc(inputFile, strlen(inputFile) + 1);
		if (inputFile == NULL) {
			printf("ERROR: Unable to allocate memory\n");
			free(exportedFile);
			exit(-1);
		}
		fd = open(inputFile,O_RDWR);
		if (fd >= 0){
			printf("ERROR : File %s already exists.You should give a filename that doesn't exist.\n",inputFile);
			close(fd);
			free(inputFile);
		}
	}while(fd >= 0);
	
	//Creating the destination of the exporting data.
	fd = open(inputFile,O_RDWR|O_CREAT,0600);
	if (fd == -1){
		printf("%s\n",strerror(errno));
		free(exportedFile);
		free(inputFile);
		exit(-1);
	}
	
	//Data transfer.
	if (lseek(fd_base,-fileLength,SEEK_CUR) == -1){
		printf("%s\n",strerror(errno));
		free(exportedFile);
		free(inputFile);
		exit(-1);
	}
	blockOfData = (char*)malloc(512*sizeof(char));
	
	if (blockOfData == NULL){
		printf("Unable to allocate memmory.\n");
		free(exportedFile);
		free(inputFile);
		exit(-1);
	}
	if (fileLength >= 512){
		oddBytes = fileLength % 512;
		numOfBlocks = (fileLength - oddBytes) /512;
		for(i = 0 ; i < numOfBlocks; i++){
			if (blocksOfData(fd_base,fd,blockOfData,512) == -1){
				free(blockOfData);
				free(exportedFile);
				free(inputFile);
				exit(-1);
			}
		}
		if (blocksOfData(fd_base,fd,blockOfData,oddBytes) == -1){
			free(blockOfData);
			free(exportedFile);
			free(inputFile);
			exit(-1);
		}
		free(blockOfData);
		free(exportedFile);
		free(inputFile);
	}
	else{
		if (blocksOfData(fd_base,fd,blockOfData,fileLength) == -1){
			free(blockOfData);
			free(exportedFile);
			free(inputFile);
			exit(-1);
		}
		free(blockOfData);
		free(exportedFile);
		free(inputFile);
	}
	if(fsync(fd_base)==-1){
		
		free(blockOfData);
		free(exportedFile);
		free(inputFile);
		printf("%s\n",strerror(errno));
		exit(-1);
	}
	printf("********************\n");
	printf("The file exported successfully!\n");
}

void delete(int fd_base){
	
	char *ID = NULL, *fileName = NULL, *givenName, *blockOfData;
	struct stat fileInfo;
	off_t sizeOfDeletedFile,sizeOfFile;
	off_t fileLength;
	ssize_t read_value,write_value;
	
	
	printf("Type the filename of the file that you want to delete:\n");
	givenName = (char*)malloc(256*sizeof(char));
	if (givenName == NULL){
		printf("Unable to allocate memmory");
		exit(-1);
	}
	
	scanf(" %255s",givenName);
	givenName = (char*)realloc(givenName,strlen(givenName)+1);
	if (givenName == NULL){
		printf("Unable to allocate memmory.\n");
		exit(-1);
	}
	if (lseek(fd_base,5,SEEK_SET) == -1){
		printf("%s\n",strerror(errno));
		exit(-1);
	}
	//Searching the file name in the database.
	do{
		fileLength = search_file(fd_base,&ID,&fileName);
		if (fileLength == -1){
			free(givenName);
			exit(-1);
		}
		if (fileLength == -2){
			printf("File %s has not been found in database!\n",givenName);
			free(ID);
			return;
		}
		if (strcmp(fileName , givenName)== 0){
			printf("File %s found in the database.\n",fileName);
			free(ID);
			break;
		}
	}while(1);
	
	//Calculating the size of the data that are gonna be deleted.
	sizeOfDeletedFile = fileLength + (sizeof(off_t)+strlen(givenName)+5*sizeof(char));
	blockOfData = (char*)malloc(512*sizeof(char));
	if (blockOfData == NULL){
		printf("Unable to allocate memmory.\n");
		free(givenName);
		exit(-1);
	}
	
	//Overwritting the data that we want to delete with useful
	//data for the database. 
	do{
		read_value = read(fd_base,blockOfData,512);
		if (read_value == -1){
			printf("%s\n",strerror(errno));
			free(blockOfData);
			free(givenName);
			exit(-1);		
		}
	
		if (lseek(fd_base,-(read_value+sizeOfDeletedFile),SEEK_CUR) == -1){
			printf("%s\n",strerror(errno));
			free(blockOfData);
			free(givenName);
			exit(-1);
		}
		
		write_value = write(fd_base,blockOfData,read_value);
		if (write_value == -1){
			printf("%s\n",strerror(errno));
			free(blockOfData);
			free(givenName);
			exit(-1);		
		}
		if (lseek(fd_base,sizeOfDeletedFile,SEEK_CUR) == -1){
			printf("%s\n",strerror(errno));
			free(blockOfData);
			free(givenName);
			exit(-1);
		}
		
		if (read_value < 512){
			free(blockOfData);
			break;
		}
	}while(1);
	if(fsync(fd_base)==-1){
		printf("%s\n",strerror(errno));
		free(blockOfData);
		free(givenName);
		exit(-1);
	}
	if (fstat(fd_base,&fileInfo) == -1){
		printf("%s\n",strerror(errno));
		free(givenName);
		exit(-1);	
	}
	sizeOfFile = fileInfo.st_size;
	if (ftruncate(fd_base,(sizeOfFile - sizeOfDeletedFile)) == -1){
		printf("%s\n",strerror(errno));
		free(givenName);
		exit(-1);
	}
	if (lseek(fd_base,0,SEEK_SET) == -1){
		printf("%s\n",strerror(errno));
		free(givenName);
		exit(-1);
	}
	free(givenName);
	printf("********************\n");
	printf("The file deleted successfully!\n");
}

int main(int argc, char* argv[]){
	
	int fd_base;
	char action_value;
	char database_startMagicNum[]="\xfa\xce\xec\xaf\n";
	char database_endMagicNum[]="\xec\xaf\xfa\xce";
	char read_returnStart[6],read_returnEnd[]="Init";
	ssize_t read_base_value,write_base_value;
	off_t lseek_value;
	
	//Checking if the user typed enough arguments.
	if (argc != 2){
		
		printf("ERROR: Wrong number of arguments.\n");
		exit(-1);
	}
	
	//Open the file.
	//Setting offset to the beginning of the file.
	//Checking if this is a database file.
	fd_base = open(argv[1],O_CREAT|O_RDWR,0600);
	if (fd_base == -1){
		
		printf("%s\n",strerror(errno));
		exit(-1);
		
	}
	
	lseek_value = lseek(fd_base,0,SEEK_SET);
	if (lseek_value == -1){
		
		printf("%s\n",strerror(errno));
		exit(-1);
		
	}
	
	read_base_value = read(fd_base,read_returnStart,5);
	if (read_base_value == -1){
		
		printf("%s\n",strerror(errno));
		exit(-1);
		
	}
	if (read_base_value == 5){
		if(lseek(fd_base,-4,SEEK_END) == -1){
			printf("%s\n",strerror(errno));
			exit(-1);
		}
		read_base_value = read(fd_base,read_returnEnd,4);
		if (read_base_value == -1){
			printf("%s\n",strerror(errno));
			exit(-1);
		}
		
	}
	
	//If the file is empty or it has just been created.
	//The code writes the necessary magicnums to make it
	//a database file.
	if (read_base_value == 0){
		if(lseek(fd_base,0,SEEK_SET) == -1){
			printf("%s\n",strerror(errno));
			exit(-1);
		}
		
		write_base_value = write(fd_base,database_startMagicNum,5);
		if (write_base_value == -1){
			printf("%s\n",strerror(errno));
			exit(-1);
		}
		if (write_base_value < 5){
			printf("ERROR: Less bytes have been written to the file.\n");
			exit(-1);
		}
		write_base_value = write(fd_base,database_endMagicNum,4);
		if (write_base_value == -1){
			printf("%s\n",strerror(errno));
			exit(-1);
		}
		if (write_base_value < 4){
			printf("ERROR: Less bytes have been written to the file.");
			exit(-1);
		}
		if (fsync(fd_base) == -1){
			printf("%s\n",strerror(errno));
			exit(-1);
		}
		
	}

	
	//Database check
	if (((memcmp(read_returnStart ,database_startMagicNum,5) != 0)||(memcmp(read_returnEnd ,database_endMagicNum,4) != 0)) && (read_base_value != 0)){
		
		printf("ERROR: The file that you've chosen is not a database file.\n");
		exit(-1);
	}
	
	//ACTION MENU	
	do{	
		action_value = print_menu();
		if (action_value == 'i'){
			import(fd_base);
		}

		if (action_value == 'f'){
			find(fd_base);
		}
		
		if (action_value =='e'){
			export(fd_base);
		}
		
		if (action_value =='d'){
			delete(fd_base);
		}
		
		if (action_value =='q'){
			printf("Quiting program ...\n");
			break;
		}
		
	}while(1);
	
	if (close(fd_base) == -1){
		printf("%s\n",strerror(errno));
		exit(-1);
	}
	
	return 0;
}

//This function scans the data between two IDs
//It returns the length of the data between two IDs.
//It also returns the file name.
//It sets the offset exactly before the next ID.
off_t search_file(int fd_base ,char **ID,char **fileName){
	
	ssize_t read_value;
	char IDsize[5];
	off_t fileLength;
	
	//read the size of the ID or the end magic number.
	read_value = read(fd_base,IDsize,4);
	IDsize[4] = '\0';
	
	if (read_value == -1){
		printf("%s\n",strerror(errno));
		return(-1);
	}
	
	if (read_value < 4){
		printf("ERROR: Unable to read important info. Program terminated.\n");
		return(-1);
	}
	
	if (memcmp(IDsize,"\xec\xaf\xfa\xce",4) == 0){
		return(-2);
	}
	
	//Creating the ID string.
	ID = (char**)malloc(sizeof(char*));
	*ID = (char*)malloc((atoi(IDsize)+1)*sizeof(char));
	
	if (ID == NULL){
		printf("Unable to allocate memmory\n");
		exit(-1);
	}
	
	if (*ID == NULL){
		printf("Unable to allocate memmory\n");
		free(ID);
		exit(-1);
	}
	
	read_value = read(fd_base,*ID,atoi(IDsize));
	
	ID[0][atoi(IDsize)] = '\0';
	
	if (read_value == -1){
		printf("%s\n",strerror(errno));
		free(*ID);
		free(ID);
		return(-1);
	}
	
	if (read_value < atoi(IDsize)){
		printf("ERROR: Unable to read important info program terminated\n");
		free(*ID);
		free(ID);
		return(-1);
	}
	
	*fileName = strchr(*ID,'.');
		if (fileName == NULL){
		printf("ERROR: Unable to read important info program terminated\n");
		free(*ID);
		free(ID);
		return(-1);
	} 
	**fileName = '\0';
	
	*fileName = *fileName + 1;
	
	read_value = read(fd_base,&fileLength,sizeof(off_t));
	
	if (read_value == -1){
		printf("%s\n",strerror(errno));
		free(*ID);
		free(ID);
		return(-1);
	}
	
	if (read_value < sizeof(off_t)){
		printf("ERROR: Unable to read important info program terminated\n");
		free(*ID);
		free(ID);
		return(-1);
	}
	
	//setting the off exactly before the next ID.
	if (lseek(fd_base,fileLength,SEEK_CUR) == -1){
		printf("%s\n",strerror(errno));
		free(*ID);
		free(ID);
		return(-1);
	}
	
	return(fileLength);
}
//A small function that helps with data transfer
int blocksOfData(int fd_base, int fd , char* blockOfData,int fileLength){
	
	ssize_t read_value, write_value;
	
	read_value = read(fd_base,blockOfData,fileLength);
	if (read_value == -1){
		printf("%s\n",strerror(errno));
		return(-1);
	}
	
	write_value = write(fd,blockOfData,read_value);
	
	if (write_value == -1){
		printf("%s\n",strerror(errno));
		return(-1);
	}
	
	return (0);
}
