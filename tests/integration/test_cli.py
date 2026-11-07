
import subprocess
import tempfile
from pathlib import Path

# Find the compiled NurseryLedger executable.
ROOT = Path(__file__).resolve().parents[2]
APP = ROOT / "build" / "nurseryledger"


def run_nursery(commands, directory):
    """Start NurseryLedger and interact with its CLI."""
    result = subprocess.run(
        [str(APP)],
        input="\n".join(commands) + "\n",
        text=True,
        capture_output=True,
        cwd=directory,
        timeout=10
    )

    assert result.returncode == 0, result.stderr
    return result.stdout


def test_persistence():
    # Temporary directory prevents tests from touching real data.
    with tempfile.TemporaryDirectory() as directory:

        # First application session.
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "exit"
        ], directory)

        # Second application session.
        output = run_nursery([
            "children",
            "bills 101",
            "payments 101",
            "credit 101",
            "exit"
        ], directory)

        assert "Ahmed" in output
        assert "PAYMENT HISTORY" in output
        assert "BILLING HISTORY" in output

        # Check the actual CSV values, not only the CLI messages.
        data = Path(directory)
        assert "101,Ahmed,2026,11,15,15000" in (
            data / "children.csv"
        ).read_text()

        assert "101,2026,11,20,20000,cash,2026,11" in (
            data / "payments.csv"
        ).read_text()

        assert len((data / "obligations.csv").read_text().splitlines()) == 2

        print("PASS: Children, bills, and payments survive restart")



def test_duplicate_child_id():
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "register 101 Omar 2026-11-15 200.00",
            "exit"
        ], directory)

        csv_file = Path(directory) / "children.csv"
        rows = csv_file.read_text().splitlines()

        # Header + exactly one registered child.
        assert len(rows) == 2, (
            f"Expected 2 CSV lines, found {len(rows)}: {rows}"
        )

        assert rows[1] == "101,Ahmed,2026,11,15,15000", (
            f"Unexpected child record: {rows[1]}"
        )

        print("PASS: Duplicate child ID rejected")




def test_duplicate_monthly_bill():
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "bill 101 2026 11",
            "exit"
        ], directory)

        rows = (
            Path(directory) / "obligations.csv"
        ).read_text().splitlines()

        # One header + exactly one monthly bill.
        assert len(rows) == 2, (
            f"Duplicate billing detected: {rows}"
        )

        # Verify the bill belongs to the correct child and period.
        assert rows[1] == "101,2026,11,2026,11,15,15000", (
            f"Incorrect billing record: {rows[1]}"
        )

        print("PASS: Duplicate monthly billing prevented")



def test_payment_without_bill():
    """A parent cannot pay against a bill that doesn't exist."""
    with tempfile.TemporaryDirectory() as directory:
        output = run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "pay 101 2026-11-20 100.00 cash 2026 11",
            "exit"
        ], directory)

        rows = (
            Path(directory) / "payments.csv"
        ).read_text().splitlines()

        assert len(rows) == 1, (
            f"Payment was accepted without a bill: {rows}"
        )

        print("PASS: Payment without an existing bill rejected")


def test_partial_payment():
    """Paying part of a bill should leave the correct amount due."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 60.00 cash 2026 11",
            "exit"
        ], directory)

        output = run_nursery([
            "balance 101 2026 11",
            "exit"
        ], directory)

        assert "90.00" in output, (
            f"Expected 90.00 EUR outstanding, got:\n{output}"
        )

        print("PASS: Partial payment leaves 90.00 EUR outstanding")


def test_overpayment_credit():
    """Extra money should become credit, not disappear into the void."""
    with tempfile.TemporaryDirectory() as directory:
        run_nursery([
            "register 101 Ahmed 2026-11-15 150.00",
            "bill 101 2026 11",
            "pay 101 2026-11-20 200.00 cash 2026 11",
            "exit"
        ], directory)

        output = run_nursery([
            "credit 101",
            "exit"
        ], directory)

        assert "50.00" in output, (
            f"Expected 50.00 EUR credit, got:\n{output}"
        )

        print("PASS: Overpayment creates 50.00 EUR credit")



if __name__ == "__main__":
    test_persistence()
    test_duplicate_child_id()
    test_duplicate_monthly_bill()
    test_payment_without_bill()
    test_partial_payment()
    test_overpayment_credit()
