//#ifndef LABELPAINTER_H
//#define LABELPAINTER_H

//#include "board/board.h"

//class QCPLabelPainterPrivate
//{
//  Q_GADGET
//public:
//  /*!
//    TODO
//  */
//  enum AnchorMode { amRectangular    ///<
//                    ,amSkewedUpright ///<
//                    ,amSkewedRotated ///<
//                   };
//  Q_ENUMS(AnchorMode)

//  /*!
//    TODO
//  */
//  enum AnchorReferenceType { artNormal    ///<
//                             ,artTangent ///<
//                           };
//  Q_ENUMS(AnchorReferenceType)

//  /*!
//    TODO
//  */
//  enum AnchorSide { asLeft      ///<
//                    ,asRight    ///<
//                    ,asTop      ///<
//                    ,asBottom   ///<
//                    ,asTopLeft
//                    ,asTopRight
//                    ,asBottomRight
//                    ,asBottomLeft
//                   };
//  Q_ENUMS(AnchorSide)

//  explicit QCPLabelPainterPrivate(QCustomPlot *parentPlot);
//  virtual ~QCPLabelPainterPrivate();

//  // setters:
//  void setAnchorSide(AnchorSide side);
//  void setAnchorMode(AnchorMode mode);
//  void setAnchorReference(const QPointF &pixelPoint);
//  void setAnchorReferenceType(AnchorReferenceType type);
//  void setFont(const QFont &font);
//  void setColor(const QColor &color);
//  void setPadding(int padding);
//  void setRotation(double rotation);
//  void setSubstituteExponent(bool enabled);
//  void setMultiplicationSymbol(QChar symbol);
//  void setAbbreviateDecimalPowers(bool enabled);
//  void setCacheSize(int labelCount);

//  // getters:
//  AnchorMode anchorMode() const { return mAnchorMode; }
//  AnchorSide anchorSide() const { return mAnchorSide; }
//  QPointF anchorReference() const { return mAnchorReference; }
//  AnchorReferenceType anchorReferenceType() const { return mAnchorReferenceType; }
//  QFont font() const { return mFont; }
//  QColor color() const { return mColor; }
//  int padding() const { return mPadding; }
//  double rotation() const { return mRotation; }
//  bool substituteExponent() const { return mSubstituteExponent; }
//  QChar multiplicationSymbol() const { return mMultiplicationSymbol; }
//  bool abbreviateDecimalPowers() const { return mAbbreviateDecimalPowers; }
//  int cacheSize() const;

//  //virtual int size() const;

//  // non-property methods:
//  void drawTickLabel(QCPPainter *painter, const QPointF &tickPos, const QString &text);
//  void clearCache();

//  // constants that may be used with setMultiplicationSymbol:
//  static const QChar SymbolDot;
//  static const QChar SymbolCross;

//protected:
//  struct CachedLabel
//  {
//    QPoint offset;
//    QPixmap pixmap;
//  };
//  struct LabelData
//  {
//    AnchorSide side;
//    double rotation; // angle in degrees
//    QTransform transform; // the transform about the label anchor which is at (0, 0). Does not contain final absolute x/y positioning on the plot/axis
//    QString basePart, expPart, suffixPart;
//    QRect baseBounds, expBounds, suffixBounds;
//    QRect totalBounds; // is in a coordinate system where label top left is at (0, 0)
//    QRect rotatedTotalBounds; // is in a coordinate system where the label anchor is at (0, 0)
//    QFont baseFont, expFont;
//    QColor color;
//  };

//  // property members:
//  AnchorMode mAnchorMode;
//  AnchorSide mAnchorSide;
//  QPointF mAnchorReference;
//  AnchorReferenceType mAnchorReferenceType;
//  QFont mFont;
//  QColor mColor;
//  int mPadding;
//  double mRotation; // this is the rotation applied uniformly to all labels, not the heterogeneous rotation in amCircularRotated mode
//  bool mSubstituteExponent;
//  QChar mMultiplicationSymbol;
//  bool mAbbreviateDecimalPowers;
//  // non-property members:
//  QCustomPlot *mParentPlot;
//  QByteArray mLabelParameterHash; // to determine whether mLabelCache needs to be cleared due to changed parameters
//  QCache<QString, CachedLabel> mLabelCache;
//  QRect mAxisSelectionBox, mTickLabelsSelectionBox, mLabelSelectionBox;
//  int mLetterCapHeight, mLetterDescent;

//  // introduced virtual methods:
//  virtual void drawLabelMaybeCached(QCPPainter *painter, const QFont &font, const QColor &color, const QPointF &pos, AnchorSide side, double rotation, const QString &text);
//  virtual QByteArray generateLabelParameterHash() const; // TODO: get rid of this in favor of invalidation flag upon setters?

//  // non-virtual methods:
//  QPointF getAnchorPos(const QPointF &tickPos);
//  void drawText(QCPPainter *painter, const QPointF &pos, const LabelData &labelData) const;
//  LabelData getTickLabelData(const QFont &font, const QColor &color, double rotation, AnchorSide side, const QString &text) const;
//  void applyAnchorTransform(LabelData &labelData) const;
//  //void getMaxTickLabelSize(const QFont &font, const QString &text, QSize *tickLabelsSize) const;
//  CachedLabel *createCachedLabel(const LabelData &labelData) const;
//  QByteArray cacheKey(const QString &text, const QColor &color, double rotation, AnchorSide side) const;
//  AnchorSide skewedAnchorSide(const QPointF &tickPos, double sideExpandHorz, double sideExpandVert) const;
//  AnchorSide rotationCorrectedSide(AnchorSide side, double rotation) const;
//  void analyzeFontMetrics();
//};
//Q_DECLARE_METATYPE(QCPLabelPainterPrivate::AnchorMode)
//Q_DECLARE_METATYPE(QCPLabelPainterPrivate::AnchorSide)
//#endif // LABELPAINTER_H
