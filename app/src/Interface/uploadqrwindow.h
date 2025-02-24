#include <QDialog>
#include <QSize>

/**
 * @class UploadQRWindow
 * @brief Provides a dialog for uploading a QR code when the license plate number is not recognized.
 *
 * Derived from `QDialog`, this class presents a modal window with a warning icon and a message prompting the user to upload
 * a QR code image if the license plate number was not detected. The dialog includes "Upload" and "Cancel" buttons.
 * The "Upload" button triggers the file selection dialog, allowing the user to select a QR code image,
 * while the "Cancel" button simply closes the window. Once a valid file is chosen, the file path is emitted through a signal.
 */
class UploadQRWindow : public QDialog
{
	Q_OBJECT

public:
	/**
	 * @brief Constructor for the UploadQRWindow class.
	 * @details Initializes the UploadQRWindow dialog with a warning icon and a message asking the user to upload a QR code if the license plate number was not recognized.
	 *          The window contains "Upload" and "Cancel" buttons. The "Upload" button triggers the upload process, while the "Cancel" button closes the window.
	 *          The window is set to be modal and prevents interaction with other windows while open.
	 * @param[in] buttonSize The size of the buttons in the window.
	 * @param[in] parent A pointer to the parent widget (default is nullptr).
	 */
	UploadQRWindow(const QSize& buttonSize, QWidget* parent = nullptr);

private slots:
	/**
	 * @brief Handles the QR code upload action.
	 * @details This function opens a file dialog allowing the user to select an image file containing the QR code.
	 *          Once a file is selected, the dialog is closed, and the file path is emitted through the `getQRPath` signal if the path is valid.
	 * @return void
	 */
	void upload();

signals:
	void getQRPath(const QString& path);
};
