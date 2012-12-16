/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_TRADING_CURVE_H
#define QWT_PLOT_TRADING_CURVE_H

#include "qwt_global.h"
#include "qwt_plot_seriesitem.h"
#include "qwt_series_data.h"

/*!
  \brief QwtPlotTradingCurve illustrates movements in the price of a
         financial instrument over time.

  QwtPlotTradingCurve supports candlestick or bar ( OHLC ) charts
  that are used in the domain of technical analysis.
*/
class QWT_EXPORT QwtPlotTradingCurve: 
    public QwtPlotSeriesItem, QwtSeriesStore<QwtOHLCSample>
{
public:
    /*!
        \brief Symbol styles.

        The default setting is QwtPlotSeriesItem::CandleStick.
        \sa setSymbolStyle(), symbolStyle()
    */
    enum SymbolStyle
    {
        //! Nothing is displayed
        NoSymbol = -1,

        /*!
          A line on the chart shows the price range (the highest and lowest
          prices) over one unit of time, e.g. one day or one hour.
          Tick marks project from each side of the line indicating the
          opening and closing price.
         */
        Bar,

        /*!
          The range between openeing/closing price are displayed as
          a filled box. The fill brush depends on the direction of the
          price movement. The box is connected to the highest/lowest
          values by lines.
        */
        CandleStick,

        /*!
          SymbolTypes >= UserSymbol are displayed by drawUserSymbol(),
          that needs to be overloaded and implemented in derived
          curve classes.

          \sa drawUserSymbol()
        */
        UserSymbol = 100
    };

    /*!
        \brief Direction of a price movement
     */
    enum Direction
    {
        //! The closing price is higher than the openening price
        Increasing,

        //! The closing price is lower than the openening price
        Decreasing
    };

    /*!
        Attributes to modify the drawing algorithm.
        \sa setPaintAttribute(), testPaintAttribute()
    */
    enum PaintAttribute
    {
        //! Check if a symbol is on the plot canvas before painting it.
        ClipSymbols   = 0x01
    };

    //! Paint attributes
    typedef QFlags<PaintAttribute> PaintAttributes;

    explicit QwtPlotTradingCurve( const QString &title = QString::null );
    explicit QwtPlotTradingCurve( const QwtText &title );

    virtual ~QwtPlotTradingCurve();

    virtual int rtti() const;

    void setPaintAttribute( PaintAttribute, bool on = true );
    bool testPaintAttribute( PaintAttribute ) const;

    void setSamples( const QVector<QwtOHLCSample> & );
    void setSamples( QwtSeriesData<QwtOHLCSample> * );

    void setSymbolStyle( SymbolStyle style );
    SymbolStyle symbolStyle() const;

    void setSymbolPen( const QColor &, 
        qreal width = 0.0, Qt::PenStyle = Qt::SolidLine );
    void setSymbolPen( const QPen & );
    QPen symbolPen() const;

    void setSymbolBrush( Direction, const QBrush & );
    QBrush symbolBrush( Direction ) const;

    void setSymbolWidth( double width );
    double symbolWidth() const;

    virtual void drawSeries( QPainter *painter,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    virtual QRectF boundingRect() const;

    virtual QwtGraphic legendIcon( int index, const QSizeF & ) const;

protected:

    void init();

    virtual void drawSymbols( QPainter *,
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect, int from, int to ) const;

    virtual void drawUserSymbol( QPainter *,
        SymbolStyle, double symbolWidth, const QwtOHLCSample & ) const;

    virtual double scaledSymbolWidth(
        const QwtScaleMap &xMap, const QwtScaleMap &yMap,
        const QRectF &canvasRect ) const;

private:
    class PrivateData;
    PrivateData *d_data;
};

Q_DECLARE_OPERATORS_FOR_FLAGS( QwtPlotTradingCurve::PaintAttributes )

#endif