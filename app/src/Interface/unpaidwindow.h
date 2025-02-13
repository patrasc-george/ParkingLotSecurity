#include <QDialog>
#include <QSize>

/**
 * @class UnpaidWindow
 * @brief Represents a dialog that notifies the user when the parking fee has not been paid.
 *
 * Derived from `QDialog`, this class displays a warning dialog informing the user that the parking fee has not been paid.
 * The window includes a warning icon, a message label indicating the unpaid fee, and a "Close" button for closing the dialog.
 * It is modal, meaning it prevents interaction with other windows while open, ensuring the user sees the message.
 * The constructor allows customization of the button size, and the window is designed to be user-friendly for managing unpaid parking fees.
 */
class UnpaidWindow : public QDialog
{
	Q_OBJECT

public:
	/**
	 * @brief Constructor for the UnpaidWindow class.
	 * @details Initializes the UnpaidWindow dialog with a warning icon and a message indicating that the parking fee has not been paid.
	 *          The window contains a "Close" button to close the dialog. The window is set to be modal and prevents interaction with other windows while open.
	 * @param[in] buttonSize The size of the buttons in the window.
	 * @param[in] parent A pointer to the parent widget (default is nullptr).
	 */
	UnpaidWindow(const QSize& buttonSize, QWidget* parent = nullptr);
};
