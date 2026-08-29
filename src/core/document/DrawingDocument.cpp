#include "core/document/DrawingDocument.hpp"

namespace Skink::Core::Document {

DrawingDocument::DrawingDocument(QSize size)
    : m_image(size, QImage::Format_ARGB32_Premultiplied)
{
    clear();
}

QSize DrawingDocument::size() const noexcept
{
    return m_image.size();
}

const QImage& DrawingDocument::image() const noexcept
{
    return m_image;
}

QImage& DrawingDocument::image() noexcept
{
    return m_image;
}

void DrawingDocument::clear(const QColor& color)
{
    m_image.fill(color);
}

void DrawingDocument::replaceImage(const QImage& image)
{
    if (image.size() == m_image.size() && image.format() == QImage::Format_ARGB32_Premultiplied) {
        m_image = image;
        return;
    }

    m_image = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
}

} // namespace Skink::Core::Document
