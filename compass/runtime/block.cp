needle 2007;

"**
 * Basic support and API for blocks (anonymous function objects).
 *
 * Blocks are functions - they encapsulate the execution of code.
 *
 * Blocks are a core part of Compass flow control, along with polymorphism. 
 * When other languages use conditional execution of expressions (C &&)
 * or with special conditional statements (C's if and while) Compass 
 * uses blocks. Blocks are needed to have any code in a method not evaluated
 * exactly once per run of a method. 
 * 
 * So, in the Compass equivalent to an if-statement, the arguments to the
 * if-operator #?// are evaluated to return blocks, or function objects.
 * The correct block is then evaluated.
 *
 * This means that instead of putting the code to be conditionally executed
 * directly as the arguments to #?//, the conditionally executed code must
 * be an argument to #?//.
 *
 * Most blocks are created through the block literal construction,
 * where the code to be executed is placed within curly braces {}. This
 * is the equivalent of other programming languages' ""lambda"" keyword.
 *
 * These block literals also specify the number of arguments the block must
 * take, and where to store the values of those arguments for the duration
 * of the block.
 * 
 * @author Daniel Perelman (dap56@cornell.edu)
 * @since needle 2007
 *"
"package compass::block;"

"**
 * Contains common block messages.
 *
 * All block literals implement this mixin. Adding methods to this mixin
 * therefore safely adds methods to all block literals. This mixin
 * is also available for anyone to implement anything else that behaves like
 * a block.
 *
 * @mutability[immutable]
 *"
public mixin $Block;
$Block mixin $Immutable;

"**
 * A block was evaluated with the incorrect number of arguments
 *"
public exception $WrongArgumentCount #onWrongArgumentCount:;

"**
 * Evaluate me, assuming I have no argument.
 *
 * @return[user] the value of the block
 * @throws $WrongArgumentCount
 * @see #evaluateWith:
 *"
public $Block evaluate {
    evaluateWith: [];
}

"**
 * Evaluate me. This may not return if I
 * have a non-local return or if I do not return.
 *
 * Note that calling this with an empty list is the same as calling
 * <@link=#evaluate>. 
 *
 * @param[7,ro restrict] $List[user]: block arguments
 * @return[user] the value of the block
 * @throws $WrongArgumentCount
 * @see #evaluate
 *"
public $Block abstract #evaluateWith:;
$Block alias #() #evaluateWith:;

"**
 * Ask how many arguments I take.
 * 
 * @return[7,ro only] $Integer: how many arguments I take
 * @throws $VariableArguments: number of arguments is variable
 *"
public $Block abstract #argumentCount;
public exception $VariableArguments #onVariableArguments;

"**
 * Loop as long as I evaluate to true, executing the loop body each time.
 * The loop body's value is ignored.
 * 
 * @param[7,ro restrict] $Block: loop body
 *"
"public $Block whileTrue: @toRepeat {
    ()?{
        @toRepeat();
        whileTrue: @toRepeat
    }//{
        ;
    }
}"
public $Block whileTrue: @toRepeat {
    {
	()?{
	    ;
	} // {
	    ^
	};
	@toRepeat();
    } repeat;
}

$Block alias #-> #whileTrue:;
