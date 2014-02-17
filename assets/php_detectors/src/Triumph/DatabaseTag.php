<?php
/**
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
 *
 * @copyright  2013 Roberto Perpuly
 * @license    http://www.opensource.org/licenses/mit-license.php The MIT License
 */
 
/**
 * A database artifact. Triumph will use this info to populate the database credentials in the SQL Browser
 * and will also allow the user to run SQL scripts against any of these databases; all without needing the 
 * user to enter the same info in the editor.
 */
class Triumph_DatabaseTag {

	/**
	 * The drivers that the editor can recognize and query.
	 */
	const DRIVER_MYSQL = 'MYSQL';
	const DRIVER_POSTGRESQL = 'POSTGRESQL';
	const DRIVER_SQLITE = 'SQLITE';
	
	/**
	 * @var string an optional friendly name for this connection. Triumph will display this to the user.
	 */
	public $label;
	
	/**
	 * @var string one of the driver constants above
	 */
	public $driver;
	
	/**
	 * Triumph will open this file when the user opens the query browser.
	 * @var string In case of a SQLite database, the full path to the SQLite file
	 */
	public $fileName;
	
	/**
	 * Triumph will open a connection to this server when the user opens the query browser.
	 * @var string host or IP address
	 */
	public $host;
	
	/**
	 * @var int port to connect to. (Optional)
	 */
	public $port;
	
	/**
	 * @var string The schema to read (required)
	 */
	public $schema;
	
	/**
	 * User that Triumph will use when connecting. (Optional)
	 */
	public $user;
	
	/**
	 * Password that Triumph will use when connecting. (Optional)
	 */
	public $password;
	
	
	/**
	 * @param $driver string required; one of the driver constants above
	 * @param $label string optional
	 * @param $host string required
	 * @param $port int optional, can be zero; zero will mean the default port
	 * @param $schema string the schema to read (required)
	 * @param $fileName string for SQLite required. This must be a full path
	 * @param $user string optional
	 * @param $password string optional
	 */
	public function __construct($driver, $label, $host, $port, $schema, $fileName, $user, $password) {
		$this->driver = $driver;
		$this->label = $label;
		$this->host = $host;
		$this->port = $port;
		$this->schema = $schema;
		$this->fileName = $fileName;
		$this->user = $user;
		$this->password = $password;
	}

}