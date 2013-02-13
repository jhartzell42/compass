"* throws None
 * throws MessageNotUnderstood
 *"
needle 2007;

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    { foo } onEndOfChannel: {
	out printLine: 1
    }
}
