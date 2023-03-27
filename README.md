This is a program written in C++ with Visual Studio 2019 that syncs 2 folders.
It handles the case if the destination folder is deleted while executing
It lacks MD5 checks, due to Visual Studio BuildTools version issue

To run the program , open cmd, cd to the directory with the sync.exe execytable which is for now in /Debug , 
then write sync.exe <source_folder> <destination_folder> <log_file_path> <interval_seconds>
