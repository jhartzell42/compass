needle 2007;

"package compass::block::wrapper;"

public class $BlockWrapper
    !block "#block (1,ro)";

public $BlockWrapper constructor newWithBlock: !block;
public $BlockWrapper block (1,ro) {
    ^!block;
}

