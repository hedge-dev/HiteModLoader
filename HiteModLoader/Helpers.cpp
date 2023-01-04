#include "pch.h"
#include "Helpers.h"

bool CreateDirectoryRecursively(const std::string& directory) {
	static const std::string separators("/");

	// If the specified directory name doesn't exist, do our thing
	DWORD fileAttributes = GetFileAttributesA(directory.c_str());
	if (fileAttributes == INVALID_FILE_ATTRIBUTES) {

		// Recursively do it all again for the parent directory, if any
		int slashIndex = directory.find_last_of(separators);
		if (slashIndex > 0) {
			CreateDirectoryRecursively(directory.substr(0, slashIndex));
		}

		// Create the last directory on the path (the recursive calls will have taken
		// care of the parent directories by now)
		BOOL result = CreateDirectoryA(directory.c_str(), nullptr);
		if (result == FALSE) {
			return false;
		}

	}
	else { // Specified directory name already exists as a file or directory

		bool isDirectoryOrJunction =
			((fileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) ||
			((fileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0);

		if (!isDirectoryOrJunction) {
			return false;
		}
	}

	return true;
}