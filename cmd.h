namespace cmd {
	void createFile(const char* filename, int size);
	void deleteFile(const char* filename);
	void createDir(const char* dirname);
	void deleteDir(const char* dirname);
	void changeDir(const char* dirname);
	void dir();
	void cp(const char* src, const char* des);
	void sum();
	void cat(const char* filename);
}

void commandInitializer();
