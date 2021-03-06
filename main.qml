import QtQuick 2.5
import QtQuick.Window 2.2
import QtMultimedia 5.6

Item {
    anchors.fill: parent

    Video {
        id: video1

        anchors.fill: parent
        source: "file:///home/kirigaya/WorkSpace/梦幻之境.mov"
        autoPlay: true
        visible: playbackState === MediaPlayer.PlayingState
        fillMode: VideoOutput.PreserveAspectCrop

        onPositionChanged: {
            var tmp = duration - position;

            if (tmp > 0 && tmp < 1000)
                video2.play();
        }

        onStopped: {
            if (video2.playbackState != MediaPlayer.PlayingState)
                video2.play();
        }
    }

    Video {
        id: video2

        anchors.fill: video1
        source: video1.source
        autoLoad: true
        visible: playbackState === MediaPlayer.PlayingState
        fillMode: VideoOutput.PreserveAspectCrop

        onPositionChanged: {
            var tmp = duration - position;

            if (tmp > 0 && tmp < 1000)
                video1.play();
        }

        onStopped: {
            if (video1.playbackState != MediaPlayer.PlayingState)
                video1.play();
        }
    }

    function loaded(str) {
        video1.source = str;
        video2.source = str;
    }

}
