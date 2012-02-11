<?php
require_once 'PHPUnit/Framework.php';

require_once realpath(__DIR__ . '/../../src/php_frameworks/MvcEditorCallClass.php');

/**
 * Test class for MvcEditorCallClass.
 */
class MvcEditorCallClassTest extends PHPUnit_Framework_TestCase
{
    /**
     * @var MvcEditorCallClass
     */
    protected $object;
	
	/**
	 * @var resource temporary file to use in testing; will contain the call stack we are testing
	 */
	protected $file;

    protected function setUp() {
        $this->object = new MvcEditorCallClass;
		$this->file = tmpfile();
		fwrite($this->file, <<<'EOF'
METHOD,view,CI_Loader::view,"index","$data"
FUNCTION,stripos,stripos,"$data","find me"
EOF
		);
		fseek($this->file, 0);
    }

    public function testClear() {
		$this->object->arguments = array(
			'news/index',
			'$data'
		);
		$this->object->resource = 'CI_Loader::view';
		$this->object->type = MvcEditorCallClass::TYPE_METHOD;
		$this->object->clear();
		$this->assertEquals('', $this->object->resource);
		$this->assertEquals('', $this->object->type);
		$this->assertEquals(TRUE, empty($this->object->arguments));
	}

    public function testFromFile() {
		
		// first line
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::TYPE_METHOD, $this->object->type);
		$this->assertEquals('CI_Loader::view', $this->object->resource);
		$this->assertEquals(2, count($this->object->arguments));
		$this->assertEquals('index', $this->object->arguments[0]);
		$this->assertEquals('$data', $this->object->arguments[1]);
		
		// second line
		$this->assertEquals(TRUE, $this->object->fromFile($this->file));
		$this->assertEquals(MvcEditorCallClass::TYPE_FUNCTION, $this->object->type);
		$this->assertEquals('stripos', $this->object->resource);
		$this->assertEquals(2, count($this->object->arguments));
		$this->assertEquals('$data', $this->object->arguments[0]);
		$this->assertEquals('find me', $this->object->arguments[1]);
		
		// no more lines, check that file pointer was advanced past the end because it shows 
		// that the file was read.
		$this->assertEquals(FALSE, $this->object->fromFile($this->file));
		$this->assertTrue(feof($this->file));
    }
}
?>
