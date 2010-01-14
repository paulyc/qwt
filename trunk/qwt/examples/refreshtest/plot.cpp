#include <stdlib.h>
#include <qwt_painter.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_layout.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include "plot.h"
#include "circularbuffer.h"
#include "settings.h"

static double wave(double x) 
{
    const double period = 1.0;
    const double c = 5.0;

    double v = ::fmod(x, period);

    const double amplitude = qwtAbs(x - qRound(x / c) * c) / ( 0.5 * c );
    v = amplitude * ::sin(v / period * 2 * M_PI);

    return v;
}

static double noise(double)
{
    return 2.0 * ( rand() / ((double)RAND_MAX + 1) ) - 1.0;
}

Plot::Plot(QWidget *parent):
    QwtPlot(parent),
    d_interval(10.0), // seconds
    d_timerId(-1)
{
    // Assign a title
    setTitle("Testing Refresh Rates");

    setCanvasBackground(Qt::white);

    alignScales();

    // Insert grid
    d_grid = new QwtPlotGrid();
    d_grid->attach(this);

    // Insert curve
    d_curve = new QwtPlotCurve("Data Moving Right");
    d_curve->setPen(QPen(Qt::black));
    d_curve->setData(CircularBuffer(d_interval, 10));
    d_curve->attach(this);

    // Axis 
    setAxisTitle(QwtPlot::xBottom, "Seconds");
    setAxisScale(QwtPlot::xBottom, -d_interval, 0.0);

    setAxisTitle(QwtPlot::yLeft, "Values");
    setAxisScale(QwtPlot::yLeft, -1.0, 1.0);
    
    d_clock.start();

    setSettings(d_settings);
}

//
//  Set a plain canvas frame and align the scales to it
//
void Plot::alignScales()
{
    // The code below shows how to align the scales to
    // the canvas frame, but is also a good example demonstrating
    // why the spreaded API needs polishing.

    canvas()->setFrameStyle(QFrame::Box | QFrame::Plain );
    canvas()->setLineWidth(1);

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scaleWidget = (QwtScaleWidget *)axisWidget(i);
        if ( scaleWidget )
            scaleWidget->setMargin(0);

        QwtScaleDraw *scaleDraw = (QwtScaleDraw *)axisScaleDraw(i);
        if ( scaleDraw )
            scaleDraw->enableComponent(QwtAbstractScaleDraw::Backbone, false);
    }

    plotLayout()->setAlignCanvasToScales(true);
}

void Plot::setSettings(const Settings &s)
{
    if ( d_timerId >= 0 )
        killTimer(d_timerId);

    d_timerId = startTimer(s.updateInterval);

    d_grid->setPen(s.grid.pen);
    d_grid->setVisible(s.grid.pen.style() != Qt::NoPen);

    CircularBuffer &buffer = (CircularBuffer &)d_curve->data();
    if ( s.curve.numPoints != buffer.size() ||
        s.curve.functionType != d_settings.curve.functionType )
    {

        switch(s.curve.functionType)
        {
            case Settings::Wave:
                buffer.setFunction(wave);
                break;
            case Settings::Noise:
                buffer.setFunction(noise);
                break;
            default:
                buffer.setFunction(NULL);
        }

        buffer.fill(d_interval, s.curve.numPoints);
    }

    d_curve->setPen(s.curve.pen);
    d_curve->setBrush(s.curve.brush);

    d_curve->setPaintAttribute(QwtPlotCurve::PaintFiltered,
        s.curve.paintAttributes & QwtPlotCurve::PaintFiltered);
    d_curve->setPaintAttribute(QwtPlotCurve::ClipPolygons,
        s.curve.paintAttributes & QwtPlotCurve::ClipPolygons);
#if QT_VERSION >= 0x040000
    d_curve->setRenderHint(QwtPlotCurve::RenderAntialiased,
        s.curve.renderHint & QwtPlotCurve::RenderAntialiased);
#endif

#if QT_VERSION >= 0x040000
    canvas()->setAttribute(Qt::WA_PaintOnScreen, s.canvas.paintOnScreen);
#endif
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintCached, s.canvas.cached);
    canvas()->setPaintAttribute(QwtPlotCanvas::PaintPacked, s.canvas.cached);

    QwtPainter::setDeviceClipping(s.canvas.deviceClipping);
    QwtPainter::setPolylineSplitting(s.curve.lineSplitting);

    d_settings = s;
}

void Plot::timerEvent(QTimerEvent *)
{
    CircularBuffer &buffer = (CircularBuffer &)d_curve->data();
    buffer.setReferenceTime(d_clock.elapsed() / 1000.0);

    // the axes are unchanged. So all we need to do
    // is to erase and repaint the content of the canvas 
    // ( without the frame )

    switch(d_settings.updateType)
    {
        case Settings::UpdateCanvas:
        {
            canvas()->invalidatePaintCache();
            canvas()->repaint(canvas()->contentsRect());
            break;
        }
        case Settings::RepaintCanvas:
        {
            canvas()->invalidatePaintCache();
            canvas()->update(canvas()->contentsRect());
            break;
        }
        default:
        {
            replot();
        }
    }
}
