#include "playlistmodel.h"
#include <QDebug>

PlaylistModel::PlaylistModel(QObject *parent) : QAbstractListModel(parent) {}

QHash<int, QByteArray> PlaylistModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TrackRole] = "track";
    roles[TitleRole] = "title";
    roles[CommentRole] = "comment";
    roles[GenreRole] = "genre";
    roles[ArtistRole] = "artist";
    roles[AlbumRole] = "album";
    roles[LengthRole] = "lengthString";
    roles[YearRole] = "year";
    roles[DateAddedRole] = "dateAdded";
    roles[IdRole] = "id";
    return roles;
}

// WARNING: The docs say something about table based models should return 0
// if parent is valid. Ignore that. The TableView in QML doesn't use a table
// based model. It uses a ListModel so we must return the proper count here.
int PlaylistModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return list_.length();
}

Qt::ItemFlags PlaylistModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

QVariant PlaylistModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 or index.row() >= list_.count()) return QVariant();

    Track track = list_[index.row()];
    switch (role) {
        case TrackRole:
            return QVariant(track.track_);
            break;
        case TitleRole:
            return QVariant(track.title_);
            break;
        case CommentRole:
            return QVariant(track.comment_);
            break;
        case GenreRole:
            return QVariant(track.genre_);
            break;
        case ArtistRole:
            return QVariant(track.artist_);
            break;
        case AlbumRole:
            return QVariant(track.album_);
            break;
        case LengthRole:
            return QVariant(track.length_);
            break;
        case YearRole:
            return QVariant(track.year_);
            break;
        case DateAddedRole:
            return QVariant(track.dateAdded_);
            break;
        case IdRole:
            return QVariant(track.id_);
            break;
        default:
            return QVariant();
    }
}

bool PlaylistModel::setData(const QModelIndex &index,
                            const QVariant &value,
                            int role)
{
    Q_UNUSED(index)
    Q_UNUSED(value)
    Q_UNUSED(role)
    emit dataChanged(createIndex(0, 0), createIndex(rowCount(), 0));
    return false;
}

void PlaylistModel::append(Track track)
{
    if (track.mrl_.isEmpty()) return;
    track.id_ = list_.length();
    // NOTE: This has to have an empty QModelIndex as first argument.
    // Because reasons, that's why.
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    list_.append(track);
    endInsertRows();
    qDebug() << "ZZZ ROW COUNT" << rowCount();
}

int PlaylistModel::count() const { return rowCount(); }

void PlaylistModel::move(int from, int to)
{
    if (from == to) return;

    qDebug() << "MOVING ROW from" << from << "to" << to;
    list_.swap(from, to);

    // Not using beginMoveRows/endMoveRows here because it would crash the app.
    emit dataChanged(createIndex(0, 0), createIndex(rowCount(), 0));
}

void PlaylistModel::remove(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    list_.removeAt(row);
    endRemoveRows();
}

QJsonObject PlaylistModel::get(int row) { return list_.at(row).toJson(); }

void PlaylistModel::clear()
{
    beginRemoveRows(QModelIndex(), 0, list_.count() - 1);
    list_ = QList<Track>();
    endRemoveRows();
}

void PlaylistModel::append(const QJsonObject &json) { append(Track(json)); }

void PlaylistModel::sort(int col, Qt::SortOrder order)
{
    // FIXME: The column most likely doesn't match the role because columns can
    // be arbitrarily ordered.
    auto func = &PlaylistModel::sortTrackAsc;
    switch (col) {
        case TrackRole:
            if (order == Qt::AscendingOrder) {
                func = &PlaylistModel::sortTrackAsc;
            } else {
                func = &PlaylistModel::sortTrackDesc;
            }
            break;
        case TitleRole:
            if (order == Qt::AscendingOrder) {
                func = &PlaylistModel::sortTitleAsc;
            } else {
                func = &PlaylistModel::sortTitleDesc;
            }
            break;
        case GenreRole:
            if (order == Qt::AscendingOrder) {
                func = &PlaylistModel::sortGenreAsc;
            } else {
                func = &PlaylistModel::sortGenreDesc;
            }
            break;
        case ArtistRole:
            if (order == Qt::AscendingOrder) {
                func = &PlaylistModel::sortArtistAsc;
            } else {
                func = &PlaylistModel::sortArtistDesc;
            }
            break;
        case AlbumRole:
            if (order == Qt::AscendingOrder) {
                func = &PlaylistModel::sortAlbumAsc;
            } else {
                func = &PlaylistModel::sortAlbumDesc;
            }
            break;
    }
    std::sort(list_.begin(), list_.end(), func);

    emit dataChanged(createIndex(0, 0), createIndex(rowCount(), 0));
    Q_UNUSED(col)
    Q_UNUSED(order)
}

bool PlaylistModel::sortTrackAsc(Track t1, Track t2)
{
    return t1.track_ < t2.track_;
}
bool PlaylistModel::sortTrackDesc(Track t1, Track t2)
{
    return t1.track_ > t2.track_;
}

bool PlaylistModel::sortTitleAsc(Track t1, Track t2)
{
    return t1.title_ < t2.title_;
}
bool PlaylistModel::sortTitleDesc(Track t1, Track t2)
{
    return t1.title_ > t2.title_;
}

bool PlaylistModel::sortGenreAsc(Track t1, Track t2)
{
    return t1.genre_ < t2.genre_;
}
bool PlaylistModel::sortGenreDesc(Track t1, Track t2)
{
    return t1.genre_ > t2.genre_;
}

bool PlaylistModel::sortArtistAsc(Track t1, Track t2)
{
    return t1.artist_ < t2.artist_;
}
bool PlaylistModel::sortArtistDesc(Track t1, Track t2)
{
    return t1.artist_ > t2.artist_;
}

bool PlaylistModel::sortAlbumAsc(Track t1, Track t2)
{
    return t1.album_ < t2.album_;
}
bool PlaylistModel::sortAlbumDesc(Track t1, Track t2)
{
    return t1.album_ > t2.album_;
}
