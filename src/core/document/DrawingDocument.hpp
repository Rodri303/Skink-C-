#pragma once

#include <QColor>
#include <QImage>
#include <QSize>

namespace Skink::Core::Document {

class DrawingDocument final {
public:
    explicit DrawingDocument(QSize size = QSize(1200, 800));

    [[nodiscard]] QSize size() const noexcept;
    [[nodiscard]] const QImage& image() const noexcept;
    [[nodiscard]] QImage& image() noexcept;

    void clear(const QColor& color = Qt::transparent);
    void replaceImage(const QImage& image);

private:
    QImage m_image;
};

} // namespace Skink::Core::Document
