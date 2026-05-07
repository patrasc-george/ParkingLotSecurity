#include "ticketprinter.h"

#include <QPrinter>
#include <QPainter>
#include <QImage>
#include <QByteArray>

#ifdef Q_OS_WIN
#include <windows.h>
#include <winspool.h>
#endif

#ifdef Q_OS_WIN
static bool feedPaperRaw(const QString& printerName, const int& lines)
{
	HANDLE handlePrinter = nullptr;
	if (!OpenPrinterW(const_cast<LPWSTR>(reinterpret_cast<LPCWSTR>(printerName.utf16())), &handlePrinter, nullptr))
		return false;

	DOC_INFO_1W docInfo;
	docInfo.pDocName = const_cast<LPWSTR>(L"Feed paper");
	docInfo.pOutputFile = nullptr;
	docInfo.pDatatype = const_cast<LPWSTR>(L"RAW");

	if (!StartDocPrinterW(handlePrinter, 1, reinterpret_cast<LPBYTE>(&docInfo)))
	{
		ClosePrinter(handlePrinter);
		return false;
	}

	if (!StartPagePrinter(handlePrinter))
	{
		EndDocPrinter(handlePrinter);
		ClosePrinter(handlePrinter);
		return false;
	}

	QByteArray data;
	for (int i = 0; i < lines; i++)
		data.append('\n');

	DWORD written = 0;
	BOOL ok = WritePrinter(handlePrinter, data.data(), static_cast<DWORD>(data.size()), &written);

	EndPagePrinter(handlePrinter);
	EndDocPrinter(handlePrinter);
	ClosePrinter(handlePrinter);

	return ok && written == static_cast<DWORD>(data.size());
}
#endif

bool TicketPrinter::printTicket(const QString& imagePath)
{
	QString printerName = "POS-80 (copy 1)";
	QImage image(imagePath);
	if (image.isNull())
		return false;

	QPrinter printer(QPrinter::HighResolution);
	printer.setPrinterName(printerName);
	printer.setColorMode(QPrinter::GrayScale);
	printer.setFullPage(true);

	QPainter painter;
	if (!painter.begin(&printer))
		return false;

	QRectF pageRect = printer.pageRect(QPrinter::DevicePixel);
	QImage scaledImage = image.scaledToWidth(static_cast<int>(pageRect.width()), Qt::SmoothTransformation);

	int x = static_cast<int>((pageRect.width() - scaledImage.width()) / 2);
	int y = 0;

	painter.drawImage(x, y, scaledImage);
	painter.end();

#ifdef Q_OS_WIN
	feedPaperRaw(printerName, 6);
#endif

	return true;
}
