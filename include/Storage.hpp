
#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "Ledger.hpp"
#include <filesystem>

class Storage {
public:
    bool save(const Ledger& ledger);
    bool load(Ledger& ledger);

private:
    bool writeFiles(
        const Ledger& ledger,
        const std::filesystem::path& directory
    );

	bool saveSnapshot(const Ledger& ledger);


	bool loadFiles(
   	     Ledger& ledger,
  	     const std::filesystem::path& directory
	);
};

#endif

