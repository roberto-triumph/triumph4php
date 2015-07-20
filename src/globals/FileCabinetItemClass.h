/*
 * @copyright  2014 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 *
 * This software is released under the terms of the MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef SRC_GLOBALS_FILECABINETITEMCLASS_H_
#define SRC_GLOBALS_FILECABINETITEMCLASS_H_

#include <globals/Sqlite.h>
#include <globals/SqliteResultClass.h>

namespace t4p {
/**
 * A file cabinet item is a full path to a file or
 * directory that the user has chosen to save
 * for later retrieval. The user may want to save
 * paths to frequently accessed files or directories
 * that are deeply nested.
 */
class FileCabinetItemClass {
	public:
	/**
	 * a unique identifier (database primary key)
	 * generated by the database, no need to set it when
	 * storing an item
	 */
	int Id;

	/**
	 * the path
	 */
	wxFileName FileName;

	FileCabinetItemClass();

	FileCabinetItemClass(const t4p::FileCabinetItemClass& src);

	t4p::FileCabinetItemClass& operator=(const t4p::FileCabinetItemClass& src);

	void Copy(const t4p::FileCabinetItemClass& src);

	/**
	 * @return bool TRUE if this item is a directory and it
	 *         exists
	 */
	bool IsDir() const;

	/**
	 * @return bool TRUE if this item is a file and it
	 *         exists
	 */
	bool IsFile() const;

	/**
	 * @return bool TRUE if this item exists
	 */
	bool Exists() const;
};

/**
 * The FileCabinetStoreClass persists a FileCabinetItem
 * instance to a sqlite database
 */
class FileCabinetStoreClass {
	public:
	FileCabinetStoreClass();

	/**
	 * persists a FileCabinetItem. this method does not
	 * throw any exceptions on database failure
	 *
	 * @param session connection to the resources sqlite file
	 * @return bool false on db failure
	 */
	bool Store(soci::session& session, t4p::FileCabinetItemClass& item);

	/**
	 * deletes an item from the cabinet
	 *
	 * @param session connection to the resources sqlite file
	 * @param int the file_cabinet_item_id the item to delete
	 * @return bool false on db failure
	 */
	bool Delete(soci::session& session, int fileCabinetItemId);
};

/**
 * Base class for file cabinet results. This class translates
 * a sql query into a FileCabinetItemclass instance.
 *
 */
class FileCabinetBaseResultClass : public t4p::SqliteResultClass {
	public:
	/**
	 * the current item of the result set. This is populated
	 * after each time the Next() method is called.
	 */
	t4p::FileCabinetItemClass Item;

	FileCabinetBaseResultClass();

	/**
	 * advance to the next row. in this method we will fill the Item instance
	 * from the current DB row.
	 */
	void Next();

	protected:
	/**
	 * binds the sql result columns to this item
	 *
	 * @param stmt the statement to execute; it must has 2 columns, file_cabinet_item_id and full_path
	 * @return bool TRUE if statement was executed successfully
	 */
	void DoBind(soci::statement& stmt);


	/**
	 * binds the sql result columns to this item and executes
	 * the statement
	 *
	 * @param stmt the statement to execute; it must has 2 columns, file_cabinet_item_id and full_path
	 * @return bool TRUE if statement was executed successfully
	 */
	bool BindAndExecute(soci::statement& stmt);

	private:
	/**
	 * the variable that is bound to the file_cabinet_item_id column of the result set
	 */
	int Id;

	/**
	 * the variable that is bound to the full_path column of the result set
	 */
	std::string FullPath;
};

/**
 * The FileCabinetResultClass reads all of the records
 * that are stored in file_cabinet table of the
 * sqlite file.
 * Results are returned in the order that they were
 * added to the cabinet, oldest first.
 */
class AllFileCabinetResultClass : public t4p::FileCabinetBaseResultClass {
	public:
	AllFileCabinetResultClass();

	protected:
	/**
	 * build the SQL to query the file_cabinet table and prepares it.
	 *
	 * @param stmt the statement to prepare and bind input parameters to.
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 * @return bool subclasses should return TRUE if there is at least one result
	 */
	bool DoPrepare(soci::statement& stmt, bool doLimit);
};

/**
 * The SingleCabinetResultClass reads a single record
 * that is stored in file_cabinet table of the
 * sqlite file. The record is chosen by ID
 */
class SingleFileCabinetResultClass : public t4p::FileCabinetBaseResultClass {
	public:
	SingleFileCabinetResultClass();

	/**
	 * @param int id the file_cabinet_id to search for
	 */
	void SetId(int id);

	protected:
	/**
	 * build the SQL to query the file_cabinet table and prepares it.
	 *
	 * @param stmt the statement to prepare and bind input parameters to.
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 * @return bool subclasses should return TRUE if there is at least one result
	 */
	bool DoPrepare(soci::statement& stmt, bool doLimit);


	private:
	/**
	 * the ID to query for
	 */
	int QueryId;
};

/**
 * The FileCabinetExactSearchResultClass searches for
 * a file cabinet item by its name. search is done
 * case-insensitive, but the entire item name
 * must match. The item's name is the file name (including
 * extension) or the name of the last directory
 */
class FileCabinetExactSearchResultClass : public t4p::FileCabinetBaseResultClass {
	public:
	FileCabinetExactSearchResultClass();

	/**
	 * @param string name the to search for
	 */
	void SetName(const std::string& name);

	protected:
	/**
	 * build the SQL to query the file_cabinet table and prepares it.
	 *
	 * @param stmt the statement to prepare and bind input parameters to.
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 * @return bool subclasses should return TRUE if there is at least one result
	 */
	bool DoPrepare(soci::statement& stmt, bool doLimit);


	private:
	/**
	 * the name to query for
	 */
	std::string Name;
};

/**
 * The FileCabinetNearMatchResultClass searches for
 * a file cabinet item by its name. search is done
 * case-insensitive, the given string must be a
 * substring of the item's full path
 */
class FileCabinetNearMatchResultClass : public t4p::FileCabinetBaseResultClass {
	public:
	FileCabinetNearMatchResultClass();

	/**
	 * @param string name the to search for
	 */
	void SetName(const std::string& name);

	protected:
	/**
	 * build the SQL to query the file_cabinet table and prepares it.
	 *
	 * @param stmt the statement to prepare and bind input parameters to.
	 * @param doLimit boolean if TRUE there should be a limit on the query
	 * @return bool subclasses should return TRUE if there is at least one result
	 */
	bool DoPrepare(soci::statement& stmt, bool doLimit);


	private:
	/**
	 * the name to query for
	 */
	std::string Name;
};
}  // namespace t4p

#endif  // SRC_GLOBALS_FILECABINETITEMCLASS_H_
