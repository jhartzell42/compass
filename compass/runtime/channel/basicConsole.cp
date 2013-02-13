needle 2007;

"** Basic I/O operations for testing. *"
"package compass::channel::basicConsole;"

private $BasicConsoleOutputChannel #primitivePut: #IOPrintChar;

private class $BasicConsoleOutputChannel
    !fd,
;

$BasicConsoleOutputChannel constructor new {
    !fd := 1
}

private $BasicConsoleOutputChannel constructor newWithFileDescriptor: !fd;

$BasicConsoleOutputChannel mixin $CharacterOutputChannel;
$BasicConsoleOutputChannel put: @char {
    primitivePut: @char codePoint
}

$BasicConsoleOutputChannel class (_) {
    ^new
}

$BasicConsoleOutputChannel printNewLine {
    primitivePut: 10
}

public class $BasicConsoleInputChannel
    !fd,
;

$BasicConsoleInputChannel mixin $CharacterInputChannel;

$BasicConsoleInputChannel constructor newWithFileDescriptor: !fd;

$BasicConsoleInputChannel constructor new {
    !fd := 0
}

$BasicConsoleInputChannel #primitiveNext #IOReadChar;

$BasicConsoleInputChannel next {
    @next := primitiveNext;
    @next < 0 and: {
	$EndOfChannelException()
    };
    ^$AsciiCharacter newWithCodePoint: @next
}

$BasicConsoleInputChannel #slurp #ByteArraySlurp;
