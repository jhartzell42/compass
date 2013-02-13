"* throws None
 * prints 3
 *"
needle 2007;

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    {
	subroutine;
	foo;
    } onException: {
	out printLine: 3
    }
}

$Test subroutine (7,ro) {
    { ^ } onException: {
	out printLine: 1
    }
}
