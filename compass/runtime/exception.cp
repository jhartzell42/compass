needle 2007;

"**
 * Basic exception support.
 *"
"package compass::exception;"

"**
 * Base class for all exceptions declared with exception declaration.
 *"
public class $Exception !message;

"**
 * Evaluate myself, keeping all exceptions from further unwinding.
 * If an exception should occur, instead of further unwinding, a custom
 * block is executed instead, passing it as an argument.
 *
 * @param[7,ro restrict] $Block: block to execute upon exception. 
 *"
"TODO [Jimmy] So, what happens when someone non-local returns past this?"
public $Block #onException: #BlockTry;

"**
 * Evaluate myself, keeping some exceptions from further unwinding,
 * using a custom block in the same manner as <@link=#onException:>.
 *
 * Whether an exception is caught here depends on a block passed in as a
 * filter. This block is executed with any exception, and if it returns
 * $true, the exception is stopped here.
 *"
public $Block onExceptionMatching: @filter do: @block {
    ^onException: { @exception --
	{
	    @value := @filter(@exception);
	} onException: {
	    @exception throw
	};
	@value ? {
	    @block(@exception)
	} // {
	    @exception throw;
	}
    }
}

"**
 * Cause myself to be signalled. Unwind the stack until an
 * <@link=#onException:> call is found, at which point that block should
 * be invoked with me for an argument. Alternatively, if a specific
 * exception class's handler-method has been called, invoke its block with
 * me as an argument.
 *
 * @receiver[7,ro restrict]
 * @return[never]
 *"
public $Exception #throw #throw;

"**
 * Create a new one of me, and throw it.
 * @return[never]
 *"
"TODO proper #() support requires collections"
"$Exception class (@args) (1,ro) {
    (new setMessage: (@args isEmpty ? {''} // {@args[0]})) throw;
}"
$Exception class #() (1,ro) {
    new throw
}

"**
 * Create and throw a new one of me, if the argument does not hold.
 *
 * @param[7,ro restrict] $Boolean: if I should throw
 * @return[self] if I don't throw
 * @return[never] if I do
 * @see #throw
 *"
$Exception class assert: @assertion {
    @assertion or: { "('assertion failed',)" () }
}

"**
 * Set message explaining the exception.
 *
 * @param[7,ro save] $String: message
 *"
$Exception setMessage: !message (1,rw);
