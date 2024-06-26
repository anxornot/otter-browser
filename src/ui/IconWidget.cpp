/**************************************************************************
* Otter Browser: Web browser controlled by the user, not vice-versa.
* Copyright (C) 2015 - 2024 Michal Dutkiewicz aka Emdek <michal@emdek.pl>
* Copyright (C) 2016 Piotr Wójcik <chocimier@tlen.pl>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
**************************************************************************/

#include "IconWidget.h"
#include "../core/ThemesManager.h"
#include "../core/Utils.h"

#include <QtCore/QEvent>
#include <QtCore/QtMath>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMenu>

namespace Otter
{

IconWidget::IconWidget(QWidget *parent) : QToolButton(parent)
{
	QMenu *menu(new QMenu(this));

	setMenu(menu);
	setToolTip(tr("Select Icon"));
	setPopupMode(QToolButton::InstantPopup);
	setMinimumSize(16, 16);
	setMaximumSize(64, 64);

	connect(menu, &QMenu::aboutToShow, this, [&]()
	{
		menu->clear();
		menu->addAction(tr("Select From File…"), this, [&]()
		{
			const QString path(QFileDialog::getOpenFileName(this, tr("Select Icon"), {}, Utils::formatFileTypes({tr("Images (*.png *.jpg *.bmp *.gif *.svg *.svgz *.ico)")})));

			if (!path.isEmpty())
			{
				setIcon(QIcon(QPixmap(path)));
			}
		});
		menu->addAction(tr("Select From Theme…"), this, [&]()
		{
			const QString name(QInputDialog::getText(this, tr("Select Icon"), tr("Icon Name:")));

			if (!name.isEmpty())
			{
				setIcon(name);
			}
		});

		if (!m_defaultIconName.isEmpty())
		{
			menu->addSeparator();
			menu->addAction(tr("Reset"), this, [&]()
			{
				setIcon(m_defaultIconName);
			})->setEnabled(Utils::savePixmapAsDataUri(icon().pixmap(16, 16)) != m_defaultIconName);
		}

		menu->addSeparator();
		menu->addAction(ThemesManager::createIcon(QLatin1String("edit-clear")), tr("Clear"), this, [&]()
		{
			setIcon(QString());
		})->setEnabled(!icon().isNull());
	});
}

void IconWidget::changeEvent(QEvent *event)
{
	QToolButton::changeEvent(event);

	if (event->type() == QEvent::LanguageChange)
	{
		setToolTip(tr("Select Icon"));
	}
}

void IconWidget::resizeEvent(QResizeEvent *event)
{
	QToolButton::resizeEvent(event);

	const int iconSize(qCeil(qMin(height(), width()) * 0.9));

	setIconSize({iconSize, iconSize});
}

void IconWidget::setIcon(const QString &name)
{
	m_iconName = name;

	QToolButton::setIcon(ThemesManager::createIcon(name));

	emit iconChanged(m_iconName);
}

void IconWidget::setIcon(const QIcon &icon)
{
	m_iconName = Utils::savePixmapAsDataUri(icon.pixmap(icon.availableSizes().value(0, {16, 16})));

	QToolButton::setIcon(icon);

	emit iconChanged(m_iconName);
}

void IconWidget::setDefaultIcon(const QString &name)
{
	m_defaultIconName = name;

	if (name.isEmpty())
	{
		setIcon(QIcon());
	}
}

QString IconWidget::getIcon() const
{
	return m_iconName;
}

int IconWidget::heightForWidth(int width) const
{
	return width;
}

bool IconWidget::hasHeightForWidth() const
{
	return true;
}

}
