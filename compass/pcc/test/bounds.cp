"* throws none
 * prints 1
 * prints 2
 *"
needle 2007;

public class $Test ($ConsoleProgram);

$Test run (7,ro) {
    {
	'abc'[3]
    } onIndexOutOfBounds: {
	out printLine: '1'
    };
    {
	'abc'[0~1~1][2]
    } onIndexOutOfBounds: {
	out printLine: '2'
    }
}
